/*
  Warnings:

  - You are about to drop the `Device` table. If the table is not empty, all the data it contains will be lost.
  - You are about to drop the `House` table. If the table is not empty, all the data it contains will be lost.
  - You are about to drop the `Room` table. If the table is not empty, all the data it contains will be lost.
  - You are about to drop the `Sensor` table. If the table is not empty, all the data it contains will be lost.

*/
-- DropForeignKey
ALTER TABLE "public"."Device" DROP CONSTRAINT "Device_roomId_fkey";

-- DropForeignKey
ALTER TABLE "public"."House" DROP CONSTRAINT "House_accountId_fkey";

-- DropForeignKey
ALTER TABLE "public"."Room" DROP CONSTRAINT "Room_houseId_fkey";

-- DropForeignKey
ALTER TABLE "public"."Sensor" DROP CONSTRAINT "Sensor_sensorId_fkey";

-- DropTable
DROP TABLE "public"."Device";

-- DropTable
DROP TABLE "public"."House";

-- DropTable
DROP TABLE "public"."Room";

-- DropTable
DROP TABLE "public"."Sensor";
