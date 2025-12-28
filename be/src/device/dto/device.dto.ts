import { ApiProperty } from "@nestjs/swagger";
import { IsNumber, IsOptional, IsString } from "class-validator";

export class DeviceDto {
    @ApiProperty({ example: "device01", description: "Device Id"})
    @IsString()
    deviceId: string;

    @ApiProperty({ example: "My device", description: "Device name"})
    @IsString()
    name?: string;

    @ApiProperty({ example: "1", description: "Account id"})
    @IsNumber()
    @IsOptional()
    accountId?: number | undefined;

    @IsString()
    @IsOptional()
    @ApiProperty({ example: "My Led", description: "Led Name"})
    nameLed?: string;
}