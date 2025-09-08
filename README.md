# Mittens

Stemming for Ruby, powered by [Snowball](https://github.com/snowballstem/snowball)

:snowflake: Supports 28 languages

[![Build Status](https://github.com/ankane/mittens/actions/workflows/build.yml/badge.svg)](https://github.com/ankane/mittens/actions)

## Installation

Add this line to your application’s Gemfile:

```ruby
gem "mittens"
```

## Getting Started

Create a stemmer

```ruby
stemmer = Mittens::Stemmer.new
```

Stem a word

```ruby
stemmer.stem("tomatoes") # "tomato"
```

## Languages

Specify the language

```ruby
stemmer = Mittens::Stemmer.new(language: "french")
```

Supports `arabic`, `armenian`, `basque`, `catalan`, `danish`, `dutch`, `dutch_porter`, `english`, `esperanto`, `estonian`, `finnish`, `french`, `german`, `greek`, `hindi`, `hungarian`, `indonesian`, `irish`, `italian`, `lithuanian`, `nepali`, `norwegian`, `porter`, `portuguese`, `romanian`, `russian`, `serbian`, `spanish`, `swedish`, `tamil`, `turkish`, and `yiddish`

## History

View the [changelog](https://github.com/ankane/mittens/blob/master/CHANGELOG.md)

## Contributing

Everyone is encouraged to help improve this project. Here are a few ways you can help:

- [Report bugs](https://github.com/ankane/mittens/issues)
- Fix bugs and [submit pull requests](https://github.com/ankane/mittens/pulls)
- Write, clarify, or fix documentation
- Suggest or add new features

To get started with development:

```sh
git clone --recursive https://github.com/ankane/mittens.git
cd mittens
bundle install
bundle exec rake compile
bundle exec rake test
```
