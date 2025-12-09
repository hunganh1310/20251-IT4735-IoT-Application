import { Module } from "@nestjs/common";
import { DeviceService } from "./device.service";
import { DeviceController } from "./device.controller";
import { MqttModule } from "src/mqtt/mqtt.module";
import { InfluxModule } from "src/influx/influx.module";
import { Prisma } from "generated/prisma";
import { PrismaModule } from "src/prisma/prisma.module";
import { EventEmitter } from "stream";
import { EventEmitterModule } from "@nestjs/event-emitter";

@Module({
    imports: [InfluxModule, PrismaModule, EventEmitterModule],
    controllers: [DeviceController],
    providers: [DeviceService],
    exports: [DeviceService],
})
export class DeviceModule {}