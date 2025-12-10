import { IsNumber, IsOptional, IsString } from "class-validator";

export class DeviceDto {
    @IsString()
    deviceId: string;

    @IsString()
    name?: string;

    @IsNumber()
    @IsOptional()
    accountId?: number | undefined;
}