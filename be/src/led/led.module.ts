import { Module } from "@nestjs/common";
import { LedController } from "./led.controller";
import { LedService } from "./led.service";

@Module({
    providers: [LedService],
    controllers: [LedController],
    exports: [LedService]
})
export class LedModule{}