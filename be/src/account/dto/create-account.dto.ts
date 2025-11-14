import { ApiProperty } from "@nestjs/swagger";
import { IsEmail, IsString, MaxLength, MinLength } from "class-validator";

export class CreateAccountDto {
    @ApiProperty({ example: "hihi", description: "Tên của người dùng" })
    @IsString()
    name: string;

    @ApiProperty({ example:"hihi@gmail.com", description: "Email của người dùng"})
    @IsEmail()
    email: string;

    @ApiProperty({ example: "123456789", description: "Password"})
    @IsString()
    @MinLength(8)
    @MaxLength(100)
    password: string;
}