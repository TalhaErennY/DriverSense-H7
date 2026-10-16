import json
import time
from collections import deque
from pathlib import Path
import numpy as np
import pyqtgraph as pg
from PyQt6.QtCore import QTimer, Qt
from PyQt6.QtWidgets import (QMainWindow,QWidget,QVBoxLayout,QHBoxLayout,QGridLayout,QLabel,QPushButton,QComboBox,QLineEdit,QPlainTextEdit,QSpinBox,QGroupBox,QMessageBox,QProgressBar)
from receiver import UDPReceiver
from recorder import SessionRecorder

HERE = Path(__file__).resolve().parent
LABEL_FILE = HERE/'config'/'labels.json'
DEFAULT_LABELS = ['Comfort-like','Autobahn-like','Track-like','Drift-like','Terrain-like']

class CollectorWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('DriverSense | Labeled Telemetry Collector')
        self.resize(1320,920)
        self.labels = self.load_labels()
        self.last_packet = None
        self.last_packet_time = 0
        self.receiver = None
        self.recorder = None
        self.view_times = deque(maxlen=800)
        self.view_speed = deque(maxlen=800)
        self.view_slip = deque(maxlen=800)
        self.build_ui()
        self.receiver = UDPReceiver('127.0.0.1',5005)
        self.receiver.packet.connect(self.got_packet)
        self.receiver.status.connect(self.status.setText)
        self.receiver.start()
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.tick)
        self.timer.start(40)

    def load_labels(self):
        if LABEL_FILE.exists():
            try:
                labels=json.loads(LABEL_FILE.read_text(encoding='utf-8'))
                if isinstance(labels,list) and all(isinstance(x,str) for x in labels):
                    return list(dict.fromkeys(x.strip() for x in labels if x.strip()))
            except (OSError,ValueError): pass
        return DEFAULT_LABELS.copy()
    def save_labels(self):
        LABEL_FILE.parent.mkdir(parents=True,exist_ok=True)
        LABEL_FILE.write_text(json.dumps(self.labels,indent=2,ensure_ascii=False),encoding='utf-8')

    def build_ui(self):
        center=QWidget(); self.setCentralWidget(center)
        root=QVBoxLayout(center)
        title=QLabel('DriverSense — Supervised Data Collection')
        title.setStyleSheet('font-size:24px;font-weight:700;padding:9px')
        root.addWidget(title)
        top=QHBoxLayout()
        self.status=QLabel('UDP başlatılıyor...');self.status.setStyleSheet('color:#69a9d7')
        self.speed_label=QLabel('0 km/h');self.rpm_label=QLabel('0 RPM');self.car_label=QLabel('Car —')
        for x in (self.status,self.speed_label,self.rpm_label,self.car_label):top.addWidget(x)
        root.addLayout(top)

        form=QGroupBox('Kayıt ayarları — sürüşe başlamadan seç')
        layout=QGridLayout(form)
        self.label_box=QComboBox();self.label_box.setEditable(True);self.label_box.addItems(self.labels)
        self.add_label=QPushButton('Etiketi kaydet');self.add_label.clicked.connect(self.add_new_label)
        self.duration=QSpinBox();self.duration.setRange(30,1800);self.duration.setSingleStep(30);self.duration.setValue(180);self.duration.setSuffix(' saniye')
        self.note=QLineEdit();self.note.setPlaceholderText('Opsiyonel: yol, trafik, hava, setup / lastik...')
        layout.addWidget(QLabel('Sürüş etiketi:'),0,0);layout.addWidget(self.label_box,0,1)
        layout.addWidget(self.add_label,0,2)
        layout.addWidget(QLabel('Kayıt süresi:'),1,0);layout.addWidget(self.duration,1,1)
        layout.addWidget(QLabel('Notlar:'),2,0);layout.addWidget(self.note,2,1,1,3)
        root.addWidget(form)

        self.vehicle_hint = QLabel('Araç tanınmadı | FH6 telemetrisinden otomatik okunacak: CarOrdinal, CarClass, PI, DrivetrainType, CarGroup')
        self.vehicle_hint.setWordWrap(True)
        root.addWidget(self.vehicle_hint)

        buttons=QHBoxLayout()
        self.start_button=QPushButton('● KAYDI BAŞLAT');self.start_button.clicked.connect(self.start_record)
        self.stop_button=QPushButton('■ KAYDI BİTİR');self.stop_button.setEnabled(False);self.stop_button.clicked.connect(lambda:self.stop_record('manual_stop'))
        self.progress=QProgressBar();self.progress.setRange(0,100);self.progress.setValue(0)
        buttons.addWidget(self.start_button);buttons.addWidget(self.stop_button);buttons.addWidget(self.progress,1)
        root.addLayout(buttons)
        self.elapsed_label=QLabel('Hazır | örnekleme 25 Hz | veri kopması >160 ms: invalid')
        root.addWidget(self.elapsed_label)
        pg.setConfigOptions(antialias=True,background='#111923',foreground='#cbd5e1')
        self.speed_plot=pg.PlotWidget(title='Speed (km/h) — last 32 seconds')
        self.speed_plot.showGrid(x=True,y=True,alpha=.25)
        self.speed_curve=self.speed_plot.plot(pen=pg.mkPen('#42cfff',width=2))
        self.slip_plot=pg.PlotWidget(title='Rear wheel slip | absolute maximum')
        self.slip_plot.showGrid(x=True,y=True,alpha=.25)
        self.slip_curve=self.slip_plot.plot(pen=pg.mkPen('#ffb454',width=2))
        root.addWidget(self.speed_plot,2);root.addWidget(self.slip_plot,2)
        self.log=QPlainTextEdit();self.log.setReadOnly(True);self.log.setMaximumHeight(116)
        root.addWidget(self.log)
        self.say('Kayıt klasörü: '+str(HERE/'recordings'))
        self.say('Öneri: her sınıf için en az 5 ayrı oturum × 180 sn; sonra çeşitliliği artır.')

    def say(self,msg):
        self.log.appendPlainText(msg)

    def add_new_label(self):
        name=self.label_box.currentText().strip()
        if not name:
            QMessageBox.warning(self,'Etiket gerekli','Boş etiket kaydedilemez.');return
        if name not in self.labels:
            self.labels.append(name);self.label_box.addItem(name)
            self.save_labels();self.say('Etiket kaydedildi: '+name)

    def got_packet(self, p, at):
        self.last_packet = p
        self.last_packet_time = at
        car_id = int(p['CarOrdinal'])
        if car_id > 0 and p.get('IsRaceOn', 0) != 0:
            drive_names = ('FWD', 'RWD', 'AWD')
            drive = int(p['DrivetrainType'])
            drive_txt = drive_names[drive] if 0 <= drive < 3 else str(drive)
            self.vehicle_hint.setText(
                f"CarOrdinal {car_id} | Class {int(p['CarClass'])} | "
                f"PI {int(p['CarPerformanceIndex'])} | {drive_txt} | "
                f"CarGroup {int(p['CarGroup'])}"
            )
        else:
            self.vehicle_hint.setText('MENÜ / ARAÇ VERİSİ YOK — kayıt beklemede')
        if car_id > 0 and p.get('IsRaceOn', 0) != 0:
            self.view_times.append(at)
            self.view_speed.append(float(p['Speed']) * 3.6)
            self.view_slip.append(max(abs(float(p['TireSlipRatioRearLeft'])),
                                      abs(float(p['TireSlipRatioRearRight']))))
        if self.recorder:
            r = self.recorder
            if car_id == 0 or p.get('IsRaceOn', 0) == 0:
                r.pause(at, 'menu_or_inactive')
            elif r.paused:
                decision, reason = r.can_resume(p)
                if decision:
                    r.resume(at)
                    r.ingest(p, at)
                    self.say('Aynı araç/konum doğrulandı, kayıt devam ediyor.')
                else:
                    self.stop_record('resume_rejected_' + reason, at)
                    self.say('Araç ya da konum değişmiş: kayıt birleştirilmedi.')
            elif not r.same_vehicle(p):
                self.stop_record('vehicle_changed', at)
                self.say('Araç değişti: kayıt sonlandırıldı.')
            else:
                r.ingest(p, at)

    def start_record(self):
        if self.recorder: return
        now=time.monotonic()
        if self.last_packet is None or now-self.last_packet_time>0.5 or self.last_packet.get('IsRaceOn',0)==0 or int(self.last_packet['CarOrdinal']) == 0:
            QMessageBox.warning(self,'Telemetri yok','Forza UDP Dash paketlerini bekle. Son 0.5 saniyede aktif paket alınmalı.');return
        label=self.label_box.currentText().strip()
        if not label:
            QMessageBox.warning(self,'Etiket gerekli','Etiket ismini gir.');return
        if label not in self.labels:
            self.labels.append(label);self.label_box.addItem(label);self.save_labels()
        try:
            self.recorder=SessionRecorder(HERE/'recordings',label,self.note.text().strip(),self.duration.value(),self.last_packet,now)
        except OSError as e:
            QMessageBox.critical(self,'Kayıt hatası',str(e));return
        # packet available before start is intentionally not recorded as new evidence
        self.start_button.setEnabled(False);self.stop_button.setEnabled(True)
        for w in (self.label_box,self.add_label,self.duration,self.note):w.setEnabled(False)
        self.say(f'Kayıt başladı: {label} | {self.recorder.folder.name}')

    def stop_record(self,reason='manual_stop',when=None):
        if not self.recorder:return
        record=self.recorder;self.recorder=None
        info=record.finish(reason,time.monotonic() if when is None else when)
        self.say(f"Bitti ({reason}): {info['valid_samples']}/{info['sample_count']} geçerli (%{100*info['valid_fraction']:.1f}) → {record.folder}")
        self.start_button.setEnabled(True);self.stop_button.setEnabled(False)
        for w in (self.label_box,self.add_label,self.duration,self.note):w.setEnabled(True)
        self.progress.setValue(0)

    def tick(self):
        now=time.monotonic()
        if self.last_packet is not None:
            self.speed_label.setText(f"{float(self.last_packet['Speed'])*3.6:.1f} km/h")
            self.rpm_label.setText(f"{float(self.last_packet['CurrentEngineRpm']):.0f} RPM")
            self.car_label.setText(f"Car {self.last_packet['CarOrdinal']}")
        if self.recorder:
            if now - self.last_packet_time > 0.5 and not self.recorder.paused:
                self.recorder.pause(now, 'udp_missing')
            self.recorder.sample_until(now)
            elapsed = min(self.recorder.active_elapsed(now), self.recorder.target_sec)
            self.progress.setValue(int(100 * elapsed / self.recorder.target_sec))
            count = self.recorder.good + self.recorder.bad
            state = 'PAUSED (menu/UDP)' if self.recorder.paused else 'REC'
            self.elapsed_label.setText(
                f"{state} {elapsed:.1f}/{self.recorder.target_sec}s | "
                f"valid {self.recorder.good}/{count} | invalid {self.recorder.bad} | "
                f"packets {self.recorder.packet_count} | pauses {len(self.recorder.pauses)}"
            )
            if elapsed >= self.recorder.target_sec:
                self.stop_record('duration_reached', now)
        elif now-self.last_packet_time>1:
            self.elapsed_label.setText('UDP paketleri bekleniyor — Forza Dash 127.0.0.1:5005')
        if self.view_times:
            xs=np.asarray(self.view_times,dtype=float)-now
            keep=xs>=-32
            self.speed_curve.setData(xs[keep],np.asarray(self.view_speed)[keep])
            self.slip_curve.setData(xs[keep],np.asarray(self.view_slip)[keep])
            self.speed_plot.setXRange(-32,0,padding=0)
            self.slip_plot.setXRange(-32,0,padding=0)

    def closeEvent(self,event):
        if self.recorder:self.stop_record('application_closed')
        self.timer.stop()
        if self.receiver:self.receiver.stop()
        event.accept()
