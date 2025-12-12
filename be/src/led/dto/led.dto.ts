import { IsInt, IsNumber, IsOptional, IsString, Max, Min } from "class-validator";

export class LedDto {
    @IsString()
    name: string;

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
    @IsString()
    @IsOptional()
    color: string

    @IsNumber()
    deviceId: number
}