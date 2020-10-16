# Enform

A ReasonReact form manager

## What it does

The goal is to simplify building and managing forms in ReasonReact by removing the bulk of the boilerplate.
Sane defaults are the rule of the day.

This is designed to be used in the with the Result(Either) monad pattern with enhanced errors
[https://github.com/LurkingFrog/Eeyo].  TODO: Move this comment to Eeyo.re If you've never seen that style
before, I found [https://fsharpforfunandprofit.com/rop/] to be extremely enlightening.

## TODO

Too many to list. Getting through "sprint 0" before really documenting this

## Packages

### Base

The primary source code for building, rendering, validation and submission.

### Derive

A ppx for generating an Enform based on a type record.

### Tests

Testing the various stuff.

### Examples

This isn't published, but shows the intended use cases for the tools.

## The book

I like the style that Rust has of making a book on using a library in addition to the straight API documentation.
Again, this is going to be a TODO, waiting for when I stabilize the API in this project
