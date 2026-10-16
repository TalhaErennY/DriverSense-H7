import csv
import json
import math
import re
import time
from datetime import datetime, timezone
from pathlib import Path
from collections import deque
from telemetry.forza_parser import FIELD_NAMES
from feature_policy import TRAINING_COLUMNS, TRAINING_FEATURES, POLICY

EXTRA = ['HostElapsedSec','ReceiveMonotonicSec','SpeedKmh','RearSlipAbsMax']
SAMPLE_EXTRA = ['ElapsedSec','SampleMonotonicSec','Valid','PacketAgeMs','SpeedKmh','RearSlipAbsMax']

def safe_slug(s):
    s = re.sub(r'[^a-zA-Z0-9_-]+', '_', s.strip())[:50].strip('_')
    return s or 'label'

class SessionRecorder:
    def __init__(self, root, label, notes, target_sec, reference_packet, now, vehicle_profile=None):
        root = Path(root)
        root.mkdir(parents=True, exist_ok=True)
        stamp = datetime.now().strftime('%Y%m%d_%H%M%S_%f')
        self.folder = root / f'{stamp}_{safe_slug(label)}'
        self.folder.mkdir(exist_ok=False)
        self.start = float(now)
        self.start_utc = datetime.now(timezone.utc).isoformat()
        self.label, self.notes = label, notes
        self.target_sec = int(target_sec)
        self.car = {k: int(reference_packet[k]) for k in ('CarOrdinal','CarClass','CarPerformanceIndex','DrivetrainType','CarGroup')}
        self.identity_keys = ('CarOrdinal','DrivetrainType','CarGroup')
        self.last_position = tuple(float(reference_packet[k]) for k in ('PositionX','PositionY','PositionZ'))
        self.position_tolerance_m = 25.0
        self.paused = False
        self.pause_started = None
        self.pauses = []
        self.paused_total = 0.0
        self.logical_origin = self.start
        self.vehicle_profile = {}  # No manually entered vehicle data
        self.raw_f = (self.folder/'raw_packets.csv').open('w',newline='',encoding='utf-8')
        self.sample_f = (self.folder/'samples_25hz.csv').open('w',newline='',encoding='utf-8')
        self.train_f = (self.folder/'train_25hz.csv').open('w',newline='',encoding='utf-8')
        self.raw_writer = csv.DictWriter(self.raw_f,fieldnames=EXTRA+FIELD_NAMES)
        self.sample_writer = csv.DictWriter(self.sample_f,fieldnames=SAMPLE_EXTRA+FIELD_NAMES)
        self.train_writer = csv.DictWriter(self.train_f, fieldnames=TRAINING_COLUMNS)
        self.raw_writer.writeheader(); self.sample_writer.writeheader(); self.train_writer.writeheader()
        self.packet_count = self.good = self.bad = 0
        self.next_index = 0
        self.queue = deque()
        self.active = None
        self.active_time = None
        self.prev_receive = None
        self.max_gap_ms = 0.0
        self.speed_min = float('inf'); self.speed_max = float('-inf')
        self.finished = False

    def active_elapsed(self, now):
        ref = self.pause_started if self.paused else float(now)
        return max(0.0, ref - self.start - self.paused_total)

    def same_vehicle(self, packet):
        # Tuning PI can vary; identity tracks ordinal, drivetrain and group.
        return all(int(packet[k]) == self.car[k] for k in self.identity_keys)

    def can_resume(self, packet):
        if not self.same_vehicle(packet):
            return False, 'vehicle_identity'
        position = tuple(float(packet[k]) for k in ('PositionX','PositionY','PositionZ'))
        if not all(math.isfinite(v) for v in position):
            return False, 'invalid_position'
        delta = math.dist(position, self.last_position)
        if delta > self.position_tolerance_m:
            return False, f'position_changed_{delta:.1f}m'
        return True, 'match'

    def pause(self, at, reason):
        if self.finished or self.paused:
            return
        # Close active portion before the paused interval. No synthetic samples during menu.
        self.sample_until(at)
        self.paused = True
        self.pause_started = float(at)
        self.queue.clear()
        self.active = None
        self.active_time = None
        self._pause_reason = reason

    def resume(self, at):
        if not self.paused or self.finished:
            return
        duration = max(0.0, float(at) - self.pause_started)
        self.pauses.append({'reason':self._pause_reason,
                            'wall_start_sec':round(self.pause_started-self.start,3),
                            'wall_duration_sec':round(duration,3),
                            'active_elapsed_sec':round(self.active_elapsed(self.pause_started),3)})
        self.paused_total += duration
        self.paused = False
        self.pause_started = None
        self.queue.clear()
        self.active = None
        self.active_time = None
        self.prev_receive = None

    def ingest(self, packet, at):
        if self.finished or self.paused or at < self.start: return
        if self.prev_receive is not None:
            self.max_gap_ms = max(self.max_gap_ms,(at-self.prev_receive)*1000)
        self.prev_receive = at
        self.packet_count += 1
        speed = float(packet['Speed'])*3.6
        slip = max(abs(float(packet['TireSlipRatioRearLeft'])),abs(float(packet['TireSlipRatioRearRight'])))
        self.last_position = tuple(float(packet[k]) for k in ('PositionX','PositionY','PositionZ'))
        self.raw_writer.writerow({'HostElapsedSec':round(self.active_elapsed(at),6),'ReceiveMonotonicSec':round(at,6),'SpeedKmh':speed,'RearSlipAbsMax':slip,**packet})
        self.queue.append((at,packet))

    def sample_until(self, now):
        # Logical 25Hz grid advances only during active driving; menus cause no rows.
        elapsed = min(self.active_elapsed(now), self.target_sec)
        while self.next_index / 25.0 <= elapsed + 1e-8:
            ts_logical = self.next_index / 25.0
            ts = self.start + self.paused_total + ts_logical
            self.next_index += 1
            while self.queue and self.queue[0][0] <= ts:
                self.active_time, self.active = self.queue.popleft()
            age = (ts - self.active_time) * 1000 if self.active_time is not None else math.inf
            valid = (self.active is not None and 0 <= age <= 160
                     and self.active.get('IsRaceOn', 0) != 0
                     and int(self.active.get('CarOrdinal', 0)) > 0)
            base={'ElapsedSec':round(ts_logical,6),'SampleMonotonicSec':round(ts,6),
                  'Valid':int(valid),'PacketAgeMs':round(age,3) if math.isfinite(age) else ''}
            if valid:
                p = self.active
                speed=float(p['Speed'])*3.6
                slip=max(abs(float(p['TireSlipRatioRearLeft'])),abs(float(p['TireSlipRatioRearRight'])))
                self.sample_writer.writerow({**base,'SpeedKmh':speed,'RearSlipAbsMax':slip,**p})
                self.train_writer.writerow({k:({**base,**p}).get(k,'') for k in TRAINING_COLUMNS})
                self.good += 1
                self.speed_min=min(self.speed_min,speed); self.speed_max=max(self.speed_max,speed)
            else:
                self.sample_writer.writerow(base)
                self.train_writer.writerow({k:base.get(k,'') for k in TRAINING_COLUMNS})
                self.bad += 1
        if self.next_index % 100 == 0:
            self.raw_f.flush(); self.sample_f.flush(); self.train_f.flush()

    def finish(self, reason, now):
        if self.finished: return
        self.sample_until(now)
        active_duration = self.active_elapsed(now)
        if self.paused:
            self.pauses.append({'reason':self._pause_reason,'wall_start_sec':round(self.pause_started-self.start,3),'wall_duration_sec':round(max(0,now-self.pause_started),3),'active_elapsed_sec':round(active_duration,3)})
        self.finished=True
        self.raw_f.flush();self.sample_f.flush()
        self.raw_f.close();self.sample_f.close();self.train_f.close()
        meta={
            'schema_version':3,'label':self.label,'notes':self.notes,
            'vehicle':self.car,
            'vehicle_source':'FH6_Data_Out_auto',
            'vehicle_reference':self.car,
            'start_utc':self.start_utc,
            'end_utc':datetime.now(timezone.utc).isoformat(),
            'requested_duration_sec':self.target_sec,
            'actual_duration_sec':round(min(active_duration,self.target_sec),3),
            'wall_duration_sec':round(max(0,now-self.start),3),
            'paused_duration_sec':round(max(0,now-self.start-active_duration),3),
            'pause_events':self.pauses,
            'resume_position_tolerance_m':self.position_tolerance_m,
            'stop_reason':reason,'sample_hz':25,'sample_period_sec':0.04,
            'sample_method':'latest-received-packet / zero-order hold',
            'max_packet_age_ms':160,
            'raw_packet_count':self.packet_count,
            'sample_count':self.good+self.bad,'valid_samples':self.good,
            'invalid_samples':self.bad,
            'valid_fraction':round(self.good/max(self.good+self.bad,1),5),
            'max_interpacket_gap_ms':round(self.max_gap_ms,2),
            'speed_min_kmh':round(self.speed_min,2) if self.good else None,
            'speed_max_kmh':round(self.speed_max,2) if self.good else None,
            'timestamp_source':'host monotonic active time excluding pauses; Forza TimestampMS preserved, not used for matching',
            'raw_file':'raw_packets.csv','sampled_file':'samples_25hz.csv',
            'training_file':'train_25hz.csv',
            'feature_policy':'feature_policy.json',
            'notice':'Only valid=1 samples should be used for training. Split train/val/test by session and vehicle, not overlapping windows.'
        }
        (self.folder/'feature_policy.json').write_text(json.dumps(POLICY,indent=2,ensure_ascii=False),encoding='utf-8')
        (self.folder/'metadata.json').write_text(json.dumps(meta,indent=2,ensure_ascii=False),encoding='utf-8')
        return meta
