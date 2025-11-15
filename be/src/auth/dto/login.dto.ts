import { ApiProperty } from "@nestjs/swagger";
import { IsEmail, IsString, MaxLength, MinLength } from "class-validator";

export class LoginDto {
    @ApiProperty({ example: "admin@gmail.com", description: "Email của người dùng"})
    @IsEmail()
    email: string;

    @ApiProperty({ example: "123456789", description: "Password của người dùng"})
    @IsString()
    @MinLength(8)
    @MaxLength(100)
    password: string;
}