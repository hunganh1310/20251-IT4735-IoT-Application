/*
  Warnings:

  - Changed the type of `deviceId` on the `Led` table. No cast exists, the column would be dropped and recreated, which cannot be done if there is data, since the column is required.

*/
-- DropForeignKey
ALTER TABLE "public"."Led" DROP CONSTRAINT "Led_deviceId_fkey";

-- AlterTable
ALTER TABLE "Led" DROP COLUMN "deviceId",
ADD COLUMN     "deviceId" INTEGER NOT NULL;

-- CreateIndex
CREATE UNIQUE INDEX "Led_deviceId_key" ON "Led"("deviceId");

-- AddForeignKey
ALTER TABLE "Led" ADD CONSTRAINT "Led_deviceId_fkey" FOREIGN KEY ("deviceId") REFERENCES "Device"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
