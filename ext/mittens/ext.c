#include <limits.h>

#include "libstemmer.h"
#include "ruby/ruby.h"

#define GetStemmer(obj, ptr) TypedData_Get_Struct((obj), stemmer_t, &stemmer_data_type, (ptr));

typedef struct stemmer {
    struct sb_stemmer* stemmer;
} stemmer_t;

static void stemmer_free(void* ptr)
{
    stemmer_t* stemmer = (stemmer_t*) ptr;
    // safe to pass null pointer according to docs
    sb_stemmer_delete(stemmer->stemmer);
    xfree(ptr);
}

const rb_data_type_t stemmer_data_type = {
    .wrap_struct_name = "stemmer",
    .function = {
        .dfree = stemmer_free,
    },
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE stemmer_allocate(VALUE klass)
{
    stemmer_t* stemmer;
    VALUE obj = TypedData_Make_Struct(klass, stemmer_t, &stemmer_data_type, stemmer);
    stemmer->stemmer = NULL;
    return obj;
}

static VALUE stemmer_initialize(int argc, VALUE* argv, VALUE self)
{
    VALUE opts;
    rb_scan_args(argc, argv, ":", &opts);

    VALUE language = Qnil;
    const char* algorithm = "english";
    if (!NIL_P(opts)) {
        language = rb_hash_aref(opts, ID2SYM(rb_intern("language")));
        if (!NIL_P(language)) {
            Check_Type(language, T_STRING);
            algorithm = StringValueCStr(language);
        }
    }

    stemmer_t* stemmer;
    GetStemmer(self, stemmer);

    // in case called multiple times
    // TODO raise error
    sb_stemmer_delete(stemmer->stemmer);

    // if adding support for encoding, may want to change encoding returned from stem
    stemmer->stemmer = sb_stemmer_new(algorithm, NULL);
    if (stemmer->stemmer == NULL) {
        rb_raise(rb_eArgError, "unknown language: %s", algorithm);
    }

    // must be placed after last use of algorithm
    RB_GC_GUARD(language);

    return self;
}

static VALUE stemmer_stem(VALUE self, VALUE value)
{
    stemmer_t* stemmer;
    GetStemmer(self, stemmer);

    if (stemmer->stemmer == NULL)
        rb_raise(rb_eRuntimeError, "stemmer not initialized");

    Check_Type(value, T_STRING);

    const sb_symbol* word = (const sb_symbol*) StringValuePtr(value);
    long size = RSTRING_LEN(value);
    if (size > INT_MAX)
        rb_raise(rb_eArgError, "string exceeds max length");

    const sb_symbol* pointer_out = sb_stemmer_stem(stemmer->stemmer, word, (int) size);
    int length_out = sb_stemmer_length(stemmer->stemmer);

    return rb_utf8_str_new((char*) pointer_out, (long) length_out);
}

static VALUE stemmer_languages(VALUE klass)
{
    VALUE out = rb_ary_new();

    const char** language = sb_stemmer_list();
    while (*language != NULL)
    {
        rb_ary_push(out, rb_utf8_str_new_cstr(*language));
        language++;
    }

    return out;
}

void Init_ext(void)
{
    VALUE mMittens = rb_define_module("Mittens");
    VALUE cStemmer = rb_define_class_under(mMittens, "Stemmer", rb_cObject);
    rb_define_alloc_func(cStemmer, stemmer_allocate);
    rb_define_method(cStemmer, "initialize", stemmer_initialize, -1);
    rb_define_method(cStemmer, "stem", stemmer_stem, 1);
    rb_define_singleton_method(cStemmer, "languages", stemmer_languages, 0);
}
