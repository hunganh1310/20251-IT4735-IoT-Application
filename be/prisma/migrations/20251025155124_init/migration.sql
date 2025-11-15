/*
  Warnings:

  - You are about to drop the column `createAt` on the `Account` table. All the data in the column will be lost.
  - You are about to drop the column `creatAt` on the `Sensor` table. All the data in the column will be lost.
  - You are about to drop the column `humidity` on the `Sensor` table. All the data in the column will be lost.
  - You are about to drop the column `temperature` on the `Sensor` table. All the data in the column will be lost.
  - A unique constraint covering the columns `[email]` on the table `Account` will be added. If there are existing duplicate values, this will fail.
  - Added the required column `name` to the `Account` table without a default value. This is not possible if the table is not empty.
  - Added the required column `role` to the `Account` table without a default value. This is not possible if the table is not empty.
  - Added the required column `ipAddress` to the `Sensor` table without a default value. This is not possible if the table is not empty.
  - Added the required column `name` to the `Sensor` table without a default value. This is not possible if the table is not empty.
  - Added the required column `type` to the `Sensor` table without a default value. This is not possible if the table is not empty.
  - Added the required column `unit` to the `Sensor` table without a default value. This is not possible if the table is not empty.

*/
-- CreateEnum
CREATE TYPE "Role" AS ENUM ('USER', 'ADMIN');

-- AlterTable
ALTER TABLE "Account" DROP COLUMN "createAt",
ADD COLUMN     "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
ADD COLUMN     "name" TEXT NOT NULL,
ADD COLUMN     "role" "Role" NOT NULL;

-- AlterTable
ALTER TABLE "Sensor" DROP COLUMN "creatAt",
DROP COLUMN "humidity",
DROP COLUMN "temperature",
ADD COLUMN     "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
ADD COLUMN     "ipAddress" TEXT NOT NULL,
ADD COLUMN     "name" TEXT NOT NULL,
ADD COLUMN     "type" TEXT NOT NULL,
ADD COLUMN     "unit" TEXT NOT NULL;

-- CreateTable
CREATE TABLE "House" (
    "id" SERIAL NOT NULL,
    "name" TEXT NOT NULL,
    "address" TEXT NOT NULL,
    "description" TEXT NOT NULL,
    "accountId" INTEGER NOT NULL,

    CONSTRAINT "House_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Room" (
    "id" SERIAL NOT NULL,
    "name" TEXT NOT NULL,
    "description" TEXT NOT NULL,
    "houseId" INTEGER NOT NULL,

    CONSTRAINT "Room_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Device" (
    "id" SERIAL NOT NULL,
    "name" TEXT NOT NULL,
    "type" TEXT NOT NULL,
    "pin" TEXT NOT NULL,
    "ipAddress" TEXT NOT NULL,
    "status" BOOLEAN NOT NULL,
    "lastUpdated" TIMESTAMP(3),

    CONSTRAINT "Device_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "DevicesInRoom" (
    "id" SERIAL NOT NULL,
    "deviceId" INTEGER NOT NULL,
    "roomId" INTEGER NOT NULL,
    "accountId" INTEGER NOT NULL,

    CONSTRAINT "DevicesInRoom_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "SensorsInRoom" (
    "id" SERIAL NOT NULL,
    "sensorId" INTEGER NOT NULL,
    "roomId" INTEGER NOT NULL,
    "accountId" INTEGER NOT NULL,

    CONSTRAINT "SensorsInRoom_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "DevicesInRoom_deviceId_roomId_key" ON "DevicesInRoom"("deviceId", "roomId");

-- CreateIndex
CREATE UNIQUE INDEX "SensorsInRoom_sensorId_roomId_key" ON "SensorsInRoom"("sensorId", "roomId");

-- CreateIndex
CREATE UNIQUE INDEX "Account_email_key" ON "Account"("email");

-- AddForeignKey
ALTER TABLE "House" ADD CONSTRAINT "House_accountId_fkey" FOREIGN KEY ("accountId") REFERENCES "Account"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Room" ADD CONSTRAINT "Room_houseId_fkey" FOREIGN KEY ("houseId") REFERENCES "House"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "DevicesInRoom" ADD CONSTRAINT "DevicesInRoom_deviceId_fkey" FOREIGN KEY ("deviceId") REFERENCES "Device"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "DevicesInRoom" ADD CONSTRAINT "DevicesInRoom_roomId_fkey" FOREIGN KEY ("roomId") REFERENCES "Room"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "DevicesInRoom" ADD CONSTRAINT "DevicesInRoom_accountId_fkey" FOREIGN KEY ("accountId") REFERENCES "Account"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "SensorsInRoom" ADD CONSTRAINT "SensorsInRoom_sensorId_fkey" FOREIGN KEY ("sensorId") REFERENCES "Sensor"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "SensorsInRoom" ADD CONSTRAINT "SensorsInRoom_roomId_fkey" FOREIGN KEY ("roomId") REFERENCES "Room"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "SensorsInRoom" ADD CONSTRAINT "SensorsInRoom_accountId_fkey" FOREIGN KEY ("accountId") REFERENCES "Account"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
