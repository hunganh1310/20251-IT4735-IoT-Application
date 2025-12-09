import { Module } from '@nestjs/common';
import { MqttService } from './mqtt.service';
import { WebsocketModule } from '../websocket/websocket.module';
import { DeviceModule } from 'src/device/device.module';
import { InfluxModule } from 'src/influx/influx.module';

@Module({
  imports: [WebsocketModule, DeviceModule, InfluxModule],
  providers: [MqttService],
  exports: [MqttService],
})
export class MqttModule {}
