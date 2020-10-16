# Building a PPX Book

A walkthrough to create a new ReasonML/ReScript PPX project from the ground up. The docs were thin and
scattered, and required a bunch of trial and error when I started working on Enform so I wanted to document
it all for next time.

## Syntax

I decided to use [mdBook](https://github.com/rust-lang/mdBook) to create this, as Markdown is fairly simple
edit and read.

The flavor of markdown use is Commonmark and the current spec documentation can be found
[here](https://spec.commonmark.org/current/)

## Building

Download, install, and run mdBook in the root ppx-book ```mdbook build```. This will create the HTML in
the ```_build``` directory.
