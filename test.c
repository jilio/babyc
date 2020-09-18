#include <stdio.h>
#include <json-c/json.h>
#include "gap_affine/affine_wavefront_align.h"

char *load_file(char const *path)
{
  char *buffer = 0;
  long length;
  FILE *f = fopen(path, "rb");

  if (f)
  {
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = (char *)malloc((length + 1) * sizeof(char));
    if (buffer)
    {
      fread(buffer, sizeof(char), length, f);
    }
    fclose(f);
  }
  buffer[length] = '\0';

  return buffer;
}

int main(int argc, char *argv[])
{
  char *raw_json = load_file("test.json");

  struct json_object *parsed_json;
  struct json_object *name;
  struct json_object *age;
  struct json_object *friends;
  struct json_object *friend;
  size_t n_friends;

  size_t i;

  parsed_json = json_tokener_parse(raw_json);

  json_object_object_get_ex(parsed_json, "name", &name);
  json_object_object_get_ex(parsed_json, "age", &age);
  json_object_object_get_ex(parsed_json, "friends", &friends);

  printf("Name: %s\n", json_object_get_string(name));
  printf("Age: %d\n", json_object_get_int(age));

  n_friends = json_object_array_length(friends);
  printf("Found %lu friends\n", n_friends);

  for (i = 0; i < n_friends; i++)
  {
    friend = json_object_array_get_idx(friends, i);
    printf("%lu. %s\n", i + 1, json_object_get_string(friend));
  }

  // Patter & Text
  char *pattern = load_file(argv[1]);
  char *text = load_file(argv[2]);
  // Allocate MM
  mm_allocator_t *const mm_allocator = mm_allocator_new(BUFFER_SIZE_8M);
  // Set penalties
  affine_penalties_t affine_penalties = {
      .match = 0,
      .mismatch = 4,
      .gap_opening = 6,
      .gap_extension = 2,
  };
  // Init Affine-WFA
  affine_wavefronts_t *affine_wavefronts = affine_wavefronts_new_complete(
      strlen(pattern), strlen(text), &affine_penalties, NULL, mm_allocator);
  // Align
  affine_wavefronts_align(
      affine_wavefronts, pattern, strlen(pattern), text, strlen(text));
  // Display alignment
  const int score = edit_cigar_score_gap_affine(
      &affine_wavefronts->edit_cigar, &affine_penalties);
  fprintf(stderr, "  PATTERN  %s\n", pattern);
  fprintf(stderr, "  TEXT     %s\n", text);
  fprintf(stderr, "  SCORE COMPUTED %d\t", score);
  edit_cigar_print_pretty(stderr,
                          pattern, strlen(pattern), text, strlen(text),
                          &affine_wavefronts->edit_cigar, mm_allocator);
  // Free
  affine_wavefronts_delete(affine_wavefronts);
  mm_allocator_delete(mm_allocator);
}