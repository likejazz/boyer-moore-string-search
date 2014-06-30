boyer-moore-string-search
=========================

[Boyer-Moore string search algorithm](http://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string_search_algorithm) implementation in C

## Usage

To compile and execute the unit tests:

    $ make
    $ ./bm

To remove the compiled file:

    $ make clean

## Sample Output

![Sample Output](https://raw.githubusercontent.com/likejazz/boyer-moore-string-search/master/sample_output.png)!

## Description

A shifting is calculated by applying the two rules: the bad character rule and the good suffix rule. An actual shifting offset is the maximum one of them.

* `delta1` - the "Bad Character" table

This table contains an entry for every character in the alphabet. The entry for char specifies how far the pattern should be right shifted when chars found in the string and it does not match the current pattern character.

* `delta2` - the "Good Suffix" table

This table contains an entry for each character in the pattern. The entry for pattern[j] specifies how far the current string position should shift to the right when pattern[j-1] does not match the string but the suffix at pattern[j .. patlen-1] does match.
