(English version below)
# 自動發現小花

[小花](https://zh.wikipedia.org/zh-tw/Nonogram)找到霧嗄嗄？這個程式來幫你霧裡看花。

「發現小花」雖然是 NP-complete 問題，坊間大多數（非喪心病狂）的題目都能在多項式時間內完成。這些情況在本程式中也能迅速找到答案。

本程式需使用 C++11 以上版本。

## 如何輸入題目

1. 先後輸入題目橫列和直排的數量。數量必須在64以內。
2. 由上而下輸入每一橫列由左到右的數字。
3. 由左到右輸入每一直排由上而下的數字。


# Nonogram solver

Simple console program solves a given [Nonogram](https://en.wikipedia.org/wiki/Nonogram).  While the problem is proven to be NP-complete, most (sane) puzzles you're likely to find should be solvable in polynomial time, which is also covered by this program.

No external libraries are used.  Requires C++11.

## To input a puzzle

1. Enter the height and width of the puzzle.  For this program, the size should not exceed 64 in either dimension.
2. Enter the constraints of each row, from the top-most to bottom-most, each row from left to right.
3. Enter the constraints of each column, from the left-most to the right most, each column from top to bottom.

Enjoy!