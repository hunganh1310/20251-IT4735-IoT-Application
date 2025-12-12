/*
  Warnings:

  - Made the column `color` on table `Led` required. This step will fail if there are existing NULL values in that column.

*/
-- AlterTable
ALTER TABLE "Led" ALTER COLUMN "color" SET NOT NULL;
