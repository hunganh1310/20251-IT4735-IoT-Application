import { Global, Module } from '@nestjs/common';
import { PrismaService } from './prisma.service';

@Global() //  nếu để Global thì không cần import ở từng module
@Module({
  providers: [PrismaService],
  exports: [PrismaService],
})
export class PrismaModule {}
