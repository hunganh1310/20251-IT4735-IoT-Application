import { ApiProperty } from "@nestjs/swagger";
import { IsInt, IsNumber, IsOptional, IsString, Max, Min } from "class-validator";

export class LedControlDto {
    @IsString()
    @IsOptional()
    @ApiProperty({ example: "basic", description: "Mode light"})
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
    @ApiProperty({ example: "#FF0000", description: "Color Light"})
    color: string
}