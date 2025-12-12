import { Module } from "@nestjs/common";
import { DeviceService } from "./device.service";
import { DeviceController } from "./device.controller";
import { MqttModule } from "src/mqtt/mqtt.module";
import { InfluxModule } from "src/influx/influx.module";
import { Prisma } from "generated/prisma";
import { PrismaModule } from "src/prisma/prisma.module";
import { EventEmitter } from "stream";
import { EventEmitterModule } from "@nestjs/event-emitter";
import { LedModule } from "src/led/led.module";

@Module({
    imports: [InfluxModule, PrismaModule, EventEmitterModule, LedModule],
    controllers: [DeviceController],
    providers: [DeviceService],
    exports: [DeviceService],
})
export class DeviceModule {}