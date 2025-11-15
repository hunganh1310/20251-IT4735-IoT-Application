/*
  Warnings:

  - You are about to drop the `DevicesInRoom` table. If the table is not empty, all the data it contains will be lost.
  - You are about to drop the `SensorsInRoom` table. If the table is not empty, all the data it contains will be lost.
  - Added the required column `roomId` to the `Device` table without a default value. This is not possible if the table is not empty.
  - Added the required column `sensorId` to the `Sensor` table without a default value. This is not possible if the table is not empty.

*/
-- DropForeignKey
ALTER TABLE "public"."DevicesInRoom" DROP CONSTRAINT "DevicesInRoom_accountId_fkey";

-- DropForeignKey
ALTER TABLE "public"."DevicesInRoom" DROP CONSTRAINT "DevicesInRoom_deviceId_fkey";

-- DropForeignKey
ALTER TABLE "public"."DevicesInRoom" DROP CONSTRAINT "DevicesInRoom_roomId_fkey";

-- DropForeignKey
ALTER TABLE "public"."SensorsInRoom" DROP CONSTRAINT "SensorsInRoom_accountId_fkey";

-- DropForeignKey
ALTER TABLE "public"."SensorsInRoom" DROP CONSTRAINT "SensorsInRoom_roomId_fkey";

-- DropForeignKey
ALTER TABLE "public"."SensorsInRoom" DROP CONSTRAINT "SensorsInRoom_sensorId_fkey";

-- AlterTable
ALTER TABLE "Device" ADD COLUMN     "roomId" INTEGER NOT NULL;

-- AlterTable
ALTER TABLE "Sensor" ADD COLUMN     "sensorId" INTEGER NOT NULL;

-- DropTable
DROP TABLE "public"."DevicesInRoom";

-- DropTable
DROP TABLE "public"."SensorsInRoom";

-- AddForeignKey
ALTER TABLE "Device" ADD CONSTRAINT "Device_roomId_fkey" FOREIGN KEY ("roomId") REFERENCES "Room"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Sensor" ADD CONSTRAINT "Sensor_sensorId_fkey" FOREIGN KEY ("sensorId") REFERENCES "Room"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
