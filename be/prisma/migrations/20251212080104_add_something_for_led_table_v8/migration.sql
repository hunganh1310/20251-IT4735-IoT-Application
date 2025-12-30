-- DropForeignKey
ALTER TABLE "public"."Led" DROP CONSTRAINT "Led_deviceId_fkey";

-- AlterTable
ALTER TABLE "Led" ALTER COLUMN "deviceId" SET DATA TYPE TEXT;

-- AddForeignKey
ALTER TABLE "Led" ADD CONSTRAINT "Led_deviceId_fkey" FOREIGN KEY ("deviceId") REFERENCES "Device"("deviceId") ON DELETE RESTRICT ON UPDATE CASCADE;
