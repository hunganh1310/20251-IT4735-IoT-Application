/*
  Warnings:

  - Added the required column `brightness` to the `Led` table without a default value. This is not possible if the table is not empty.
  - Added the required column `color` to the `Led` table without a default value. This is not possible if the table is not empty.
  - Added the required column `led_is_on` to the `Led` table without a default value. This is not possible if the table is not empty.
  - Added the required column `led_mode` to the `Led` table without a default value. This is not possible if the table is not empty.
  - Added the required column `presence_mode_enabled` to the `Led` table without a default value. This is not possible if the table is not empty.

*/
-- AlterTable
ALTER TABLE "Led" ADD COLUMN     "brightness" INTEGER NOT NULL,
ADD COLUMN     "color" TEXT NOT NULL,
ADD COLUMN     "led_is_on" BOOLEAN NOT NULL,
ADD COLUMN     "led_mode" TEXT NOT NULL,
ADD COLUMN     "presence_mode_enabled" BOOLEAN NOT NULL;
