-- DropForeignKey
ALTER TABLE "public"."Device" DROP CONSTRAINT "Device_id_fkey";

-- AddForeignKey
ALTER TABLE "Led" ADD CONSTRAINT "Led_deviceId_fkey" FOREIGN KEY ("deviceId") REFERENCES "Device"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
