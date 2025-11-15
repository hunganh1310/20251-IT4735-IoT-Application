/*
  Warnings:

  - A unique constraint covering the columns `[home_id]` on the table `House` will be added. If there are existing duplicate values, this will fail.
  - Added the required column `home_id` to the `House` table without a default value. This is not possible if the table is not empty.

*/
-- AlterTable
ALTER TABLE "House" ADD COLUMN     "home_id" TEXT NOT NULL;

-- CreateIndex
CREATE UNIQUE INDEX "House_home_id_key" ON "House"("home_id");
