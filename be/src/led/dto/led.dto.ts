import { ApiProperty } from "@nestjs/swagger";
import { IsInt, IsNumber, IsOptional, IsString, Max, Min } from "class-validator";

export class LedDto {
    @ApiProperty({ example: "My device", description: "Device Name"})
    @IsString()
    name: string;

    @ApiProperty({ example: "basic", description: "Light Mode"})
    @IsString()
    @IsOptional()
    led_mode?: string

    @IsInt()
    @Min(-1)
    @Max(256)
    brightness?: number = 0

    @IsOptional()
    led_is_on?: boolean
    @IsOptional()
    presence_mode_enabled?: boolean 

    @ApiProperty({ example: "#FF0000", description: "Led Color"})
    @IsString()
    @IsOptional()
    color: string

    @IsNumber()
    deviceId: number
}