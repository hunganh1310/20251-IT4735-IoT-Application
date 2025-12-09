import { Body, Controller, Get, Param, Post } from "@nestjs/common";
import { DeviceService } from "./device.service";
import { DeviceDto } from "./dto/device.dto";

@Controller('devices')
export class DeviceController {
    constructor(
        private readonly deviceService: DeviceService
    ) {}

    @Get('/:device_id/data')
    getDeviceData(@Param('device_id') deviceId: string) {
        return this.deviceService.getDeviceData(deviceId);
    }

    @Get()
    getAllDeviceIds() {
        return this.deviceService.getAllDeviceIds();
    }

    @Post()
    createDevice(@Body() body: DeviceDto) {
        return this.deviceService.createDevice(body.deviceId, body.accountId, body?.name);
    }
}