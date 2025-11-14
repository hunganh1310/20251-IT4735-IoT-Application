import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';
import { ValidationPipe } from '@nestjs/common';
//import { MicroserviceOptions, Transport } from '@nestjs/microservices';
import { TransformInterceptor } from './common/interceptors/transform.interceptors';
import { DocumentBuilder, SwaggerModule } from '@nestjs/swagger';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);

  const config = new DocumentBuilder()
    .setTitle("Project Home IOT API")
    .setDescription("API documentation for Project III")
    .setVersion("1.0")
    .addBearerAuth()
    .build();
    
  const document = SwaggerModule.createDocument(app, config);
  SwaggerModule.setup("api/docs", app, document);

  // Global validation
  app.useGlobalPipes(
    new ValidationPipe({
      whitelist: true,        // Loại bỏ field không có trong DTO
      forbidNonWhitelisted: true, // Báo lỗi nếu có field lạ
      transform: true,        // Tự động transform type (ví dụ string -> number)
    }),
  );

  app.useGlobalInterceptors(new TransformInterceptor());

  // const microservice = app.connectMicroservice<MicroserviceOptions>({
  //   transport: Transport.MQTT,
  //   options: {
  //     url: 'http://localhost:1883',
  //   }
  // })

  await app.startAllMicroservices();
  await app.listen(process.env.PORT ?? 3000);
}
bootstrap();
