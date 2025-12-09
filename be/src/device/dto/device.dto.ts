import { IsNumber, IsString } from "class-validator";

export class DeviceDto {
    @IsString()
    deviceId: string;

    @IsString()
    name?: string;

    @IsNumber()
    accountId?: number | undefined;
}