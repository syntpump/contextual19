# Contextual19 Python Interpreter

Files in this directories provides 4 classes which you can use to work with Ctx19

## `ctx19.Contextual19Parser`

This is just regular parser that takes rules in object representation.

### Initialization

To initialize this class, just call it and take there rules in object representation:

```python
from ctx19 import Contextual19Parser
data = [
	{
		"if": ...,
		"then": ...
	},
	...
]
parser = Contextual19Parser(data)
```

Data you passed will be stored in `parser.data`.

### methods

* `apply(sentence: list) -> list`  
	This method apply rules, stored in `self.data` to the sentence you passed.  
	`sentence` is the list of dictionaries which represents tokens with their properties. It can look like this:
	```json
	[
		{
			"tense": "past",
			"gender": "neutral",
			"some_other_stuff": "other_value"
		},
		{
			"tense": "future",
			"voice": "active",
			"another_property": "another_value"
		}
	]
	```
	If some property will be missing while rule applying then it will be skipped.
* `save(filepath: str) -> void`  
	This method will simply save the rules stored in `self.data` into file you passed as `filepath`.

### Private methods

There's also some methods that should not be used in your code, but they are widely used in `Contextual19Parser.apply` method and they can be useful in some cases.

* `applyRule(rule: dict, token: dict) -> dict`  
	Apply assignments from some rule to the given token. This won't check if the given rule is really appliable to the token in this context. Resulting token will be returned.
* `ruleIsAppliable(rule: dict, sentence: list, token: int) -> bool`  
	Check if the given `rule` is appliable to the `token`th token in the `sentence`. It'll check the context.
* `applyIfPossible(rule: dict, sentence: list, token: int) -> list`  
	It will apply some rule to the token if it's appliable in this context. Resulting sentence will be returned.
* `applyRulesTo(rules: list, sentence: list, token: int) -> list`  
	Apply each rule in the given list to the `token`-th token in the `sentence` if they're appliable.

## `ctx19.Contextual19FileParser`

This is the children of `ctx19.Contextual19Parser` with the changed initialization method.

### Initialization

To initialize this parser just pass path to file with rules as the `filepath` parameter:

```python
from ctx19 import Contextual19FileParser
parser = Contextual19FileParser("path/to/file.ctx19")
```

Since this is the `Contextual19Parser` children, it'll inherit all the parent methods, but one new private was added.

### Private methods

* `parseFile(void) -> void`  
	This will parse Contextual19 syntax from the file saved in `self.file`. You can change the file and call this method again.

## `ctx19json.Contextual19JSONFileParser`

This is the cildren of `ctx19.Contextual19FileParser` but with the changed `parseFile` private methods, so it can read `.json` file which contains rules in object representation.

### Initialization

To initialize this parser just pass path to `.json` file:

```python
from ctx19json import Contextual19JSONFileParser
parser = Contextual19JSONFileParser("path/to/file.json")
```

**Please note,** that this class will require `json` module to be installed.

## `ctx19json.Contextual19YAMLFileParser`

This is the cildren of `ctx19.Contextual19FileParser` and it works just like JSON-reader, but with YAML.

### Initialization

To initialize this parser just pass path to `.yaml`/`.yml` file:

```python
from ctx19json import Contextual19YAMLFileParser
parser = Contextual19YAMLFileParser("path/to/file.yaml")
```

**Please note,** that this class will require `yaml` module to be installed.

