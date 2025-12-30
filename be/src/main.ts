import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';
import { ValidationPipe } from '@nestjs/common';
//import { MicroserviceOptions, Transport } from '@nestjs/microservices';
import { TransformInterceptor } from './common/interceptors/transform.interceptors';
import { DocumentBuilder, SwaggerModule } from '@nestjs/swagger';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);
  app.setGlobalPrefix('api');

  const config = new DocumentBuilder()
    .setTitle("Aquarium IoT API")
    .setDescription("API cho hệ thống IoT (ESP32, Node.js, React) Bao gồm xác thực, quản lý thiết bị và truy vấn dữ liệu lịch sử.")
    .setVersion("1.0")
    .addBearerAuth()
    .build();
    
  const document = SwaggerModule.createDocument(app, config);
  SwaggerModule.setup("api/docs", app, document);

  app.useGlobalPipes(
    new ValidationPipe({
      whitelist: true,        // Loại bỏ field không có trong DTO
      forbidNonWhitelisted: true, // Báo lỗi nếu có field lạ
      transform: true,        // Tự động transform type (ví dụ string -> number)
    }),
  );
  app.useGlobalInterceptors(new TransformInterceptor());

  app.enableCors({
    origin: 'http://localhost:5173',
    methods: 'GET,HEAD,PUT,PATCH,POST,DELETE',
    allowedHeaders: 'Content-Type, Accept, Authorization',
  })

  await app.listen(process.env.PORT ?? 3000);
}
bootstrap();
