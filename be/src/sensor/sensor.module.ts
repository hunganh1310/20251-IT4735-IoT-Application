import { Module } from '@nestjs/common';
import { SensorService } from "./sensor.service";
import { SensorController } from "./sensor.controller";
import { MqttModule } from 'src/mqtt/mqtt.module';
import { DeviceModule } from 'src/device/device.module';

@Module({
    imports: [MqttModule],
    controllers: [SensorController],
    providers: [SensorService],
})
export class SensorModule {}