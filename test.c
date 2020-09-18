#include <stdio.h>
#include <json-c/json.h>

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
}