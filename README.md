# Contextual19 Markup Standard

Contextual19 is a markup standard to save rules for transformation based tagging (the most simple example described in this [NLTK book](http://www.nltk.org/book/ch05.html)). It's just a lighter way to save them and make human-readable.

## Syntax
Every rule consists of `if` and `then` part where you can specify conditions whether token's tagging must be changed.  
Here's the syntax for rules:
```
if
  ...
then
  ...
```
`if` part consists of selectors (just like in CSS markup) and their properties:
```
if
  next
    gender is fem
    tense is past
  previous
    pos is noun
then
  ...
```
You don't need to use brackets or quotes, but no field can contain spaces. As comparison operators you can use `is` or `is not`:
```
gender is fem
voice is not active
```
Using selectors you can get oncoming and preceding context (`next` and `previous` selectors) or reach _second next/previous_ or even _n-th next/previous_ tokens using this technique:
```
if
  first next
    # Rules that will be applied to first next token
  first previous
    # Rules for first previous token
  second next/previous
    ...
  third next/previous
    ...
  fourth next/previous
    ...
  fifth next/previous
    ...
  [0-9]th next/previous
    # Using this selector you can reach n-next and n-preceding token.
    # Yes, selectors like '1th' and '22th' will be absolutely legally,
    # furthermore, endings like 'st', 'nd' and 'rd' is out of standard.
then
  # Rules to be applied
```
In order to reach the very beginning or the end of the sentence, you can use `end` and `beginning` selectors.
In `then` part you should specify new tagging properties for selected token. Use `becomes` as assigning operator:
```
if
  ...
then
  case becomes accusative
  voice becomes passive
```

## Regexing the syntax
Your code can be recognized with the following regex:
```regex
(?(DEFINE)
	(?<absolute_token> (token|beginning|end) )
	(?<num_position> (first|second|third|fourth|fifth|\d+th) )
	(?<sel_name> (previous|next) )
	(?<sel_full>
		\b(
			(?&absolute_token) |
			( (?&num_position) \s )? (?&sel_name)
		)\b
	)
	(?<comparison> (is ( \s not )? ) )
	(?<property_name> \w+(?=\s(?:is|becomes)) )
	(?<property_value> (\w+) )
	(?<property> ( (?&property_name) \s (?&comparison) \s (?&property_value) ) )
	(?<assign> (becomes) )
	(?<assigning> ( (?&property_name) \s (?&assign) \s (?&property_value) ) )
	(?<if> (if) )
	(?<then> (then) )
	(?<properties_block> ( \t{2} (?&property)\n )+ )
	(?<assignings_block> ( \t (?&assigning)\n )+ )
	(?<selector_block> \t (?&sel_full) \n (?&properties_block) )
	(?<selectors_block> (?&selector_block)+ )
	(?<if_block> ( (?&if) \n (?&selectors_block) ) )
	(?<then_block> ( (?&then) \n (?&assignings_block) ) )
	(?<rule> (?&if_block) (?&then_block) )
	(?<rules_block> ( (?&rule) \n? )* )
)
^(?&rules_block)$
```
Use it with `/gmx` flags.
You can also test it on [regex101](https://regex101.com/r/K54WEL/1/).

## Converting to another markups

### Convert to JSON (object representation)
This standard can be easily represented in object notation. Here's the example using JSON with comments.
```json
[
  {
    "if": [
      {
        "__name": "next",
        "__position": 1,
        "name": [true, "value"],
        "name": [false, "value"]
      }
    ],
    "then": {
      "name": "value"
    }
  }
]
```
1. Rules must be enclosed into array, even though there's only one rule.
2. Every rule should contain `if` and `then` properties.
3. Selectors and their comparisons must be stored in one dictionary.
4. Position of token should be written in `__position` property, inside of selector dictionary. E.g.: for `second previous` `__name` becomes `previous` and `__position` will be `2`.
5. Statement `name is value` should be encoded like `[true, "value"]` and `name is not value` becomes `[false, "value"]`.


You can also use a `tojson.c` program in the `converters` in this repo ([go there](converters/tojson.c)). Just compile it with GCC compiler and run with the following parameters:
```
tojson --file rules.ctx19 --output rules.json
```
Here `file` is a path to your `.ctx19` file and `output` is the path to file to parse in. If it's not exist, it'll be created.

### Convert to YAML
Contextual19 can be converted to YAML as well. It'll have the same structure as the JSON document. To perform it automatically, compile `toyaml.c` ([go there](converters/toyaml.c)) with GCC compiler and run with the following parameters:
```
toyaml --file rules.ctx19 --output rules.yml
```
Here parameters is the same as in paragraph above.

## Interpreters
* [Go to Python interpreter](interpreter-py)
