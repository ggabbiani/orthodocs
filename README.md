# OrthoDocs

OrthoDocs (ODOX) is a project aimed to provide a documentation generator and a static analysis tool for the OpenSCAD scripting language. Due to the 'flat' nature of the language, only a graphical representation of the inter-package relations will be provided. Among the features also a cross-reference between API items is planned. For the list of the feature planned and their status see [Project status](#project-status).

## Design

ODOX tries to:

* minimize changes to already commented code (actually only one);
* not introduce any specific documentation tag;
* focus the produced output to Markdown only.

Rather then imposing a comment style, ODOX's adopted strategy is:

1. analyse the surce code;
2. create a syntax signature for every global variable, function or module;
3. discover every annotation;
4. bind annotations to the proper syntactical entities;
5. generate documentation by integrating syntactical entities found with the correct annotations.

## Example code

Suppose to have the following code:

    // Calculates the bounding box of an origin-centered sphere
    function sphereBoundingBox(d,r=1) = 
        let(d = d ? d : 2*r) [[-r,-r,-r],[+r,+r,+r]];

ODOX will produce the following documentation:

---

### function sphereBoundingBlock

__Syntax:__

`sphereBoundingBlock(d,r=1)`

---

If you modify the existing comment in the following way

    //! Calculates the bounding box of an origin-centered sphere
    function sphereBoundingBox(d,r=1) = 
        let(d = d ? d : 2*r) [[-r,-r,-r],[+r,+r,+r]];

ODOX will interpret the comment starting with '!' as an annotation related to the function with the following result:

---

### function sphereBoundingBlock

__Syntax:__

`sphereBoundingBlock(d,r=1)`

Calculates the bounding box of an origin-centered sphere

---

If you think that parameter d needs an explanation, just comment it out:

    //! Calculates the bounding box of an origin-centered sphere
    function sphereBoundingBox(/*! this is the diameter */ d,r=1) = 
        let(d = d ? d : 2*r) [[-r,-r,-r],[+r,+r,+r]];

The result now will be:

---

### function sphereBoundingBox

__Syntax:__

`sphereBoundingBox(d,r=1)`

Calculates the bounding box of an origin-centered sphere

__Parameters:__

__d__  
this is the diameter

---

## The command line



## Project status

| Syntax      | Description |
| ----------- | ----------- |
| Header      | Title       |
| Paragraph   | Text        |

