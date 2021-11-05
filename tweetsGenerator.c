// Written by Avinoam Nukrai

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define MAX_ARGS_NUM 5
#define MIN_ARGS_NUM 4
#define FILE_P_ERROR "Error: The given path file is invalid,enter a new one.\n"
#define ADD_FUNC_ERROR "Error: add function was failed!\n"
#define ARGS_NUM_ERROR "Usage: Invalid number of arguments!\n"
#define ALLOCATION_ERROR "Allocation failure:"
#define SSCANF_ERROR "Error: sscanf function was failed!\n"

/* My program step by step:
 * to build check input validity function
 * fill the dictionary with the words from the given file
 * choose a random word to start from
 * calc the probability of each following word(to updates the structs values)
 * to build a sentences from the list of prob and to prints them
 * to free all (!!) the data
 **/

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list; // array of all the next words after word
  //... Add your own fields here
  int num_of_occur;
  int prob_list_size;
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  int num_of_next_appear; /* Number of times that the current WP is next for
 * some word in the dictionary */
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
  WordStruct *data;
  struct Node *next;
} Node;

typedef struct LinkList {
  Node *first;
  Node *last;
  int size; // the size of the linked list
} LinkList;



/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL)
  {
    return 1;
  }
  *new_node = (Node){data, NULL};

  if (link_list->first == NULL)
  {
    link_list->first = new_node;
    link_list->last = new_node;
  }
  else
  {
    link_list->last->next = new_node;
    link_list->last = new_node;
  }

  link_list->size++;
  return 0;
}
/*************************************/




/********** My debug functions ***********/


/*
 * @param dictionary
 * @return 0 and prints each item (node) of the dictionary
 */
int print_dict(LinkList* link_list)
{
    Node * node = link_list->first;
    while (node != link_list->last){
        printf("%s\n",node->data->word);
        node= node->next;}
    return 0;
}

/*
 * @param WordStruct struct
 * @return return none and prints the items of the probability list of the WS*/
void print_prob_list(WordStruct *word1){
  WordProbability *prob_array = word1->prob_list;
  int num =0;
  for (int i = 0; i < word1->prob_list_size; ++i) {
    printf("%s", word1->prob_list[i].word_struct_ptr->word);
  }
    while (num != word1->prob_list_size){
        printf("%s\n", word1->prob_list->word_struct_ptr->word);
        prob_array++;
        num++;
    }
}

/********************************************/



/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number){
  int rand_num = rand()%max_number;
  return rand_num;
}


/**
 * Get dictionary and some random int
 * @param dict, rand_num
 * @return the item (node) in the dictionary that in the index of rand_num
 */
Node *get_rand_node(LinkList *dict, int rand_num){
    Node *cur_node = dict->first;
    int num = 0;
    while (num != rand_num){
        cur_node = cur_node->next;
        num++;
    }
    return cur_node;
}


/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word(LinkList *dictionary){
    int dict_size = dictionary->size;
    int rand_num = get_random_number(dict_size);
    // first to get the word in the rand index in the dictionary
    Node *rand_node = get_rand_node(dictionary, rand_num);
    WordStruct *rand_word = rand_node->data;
    long int index = strlen(rand_word->word) - 1;
    while (rand_word->word[index] == '.'){
        // to chose a new rand word!
        rand_num = get_random_number(dict_size);
        rand_word = (get_rand_node(dictionary, rand_num))->data;
        index = strlen(rand_word->word) - 1;
    }
    return rand_word;
}


/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * How it works? So the func creat an array of chars that contain each
 * next_word (word that is in word_struct_ptr.prob_list) times of
 * word.num_of_next_appear and then the func calc the random number between
 * 0 - sizeof(array) and then just search for the WordStruct in the prob_list
 * that contain the random_word and return it.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr) {
  char *all_next[MAX_WORD_LENGTH * MAX_SENTENCE_LENGTH];
  int dict_size = word_struct_ptr->prob_list_size;
  int index = 0, i = 0;
  while (i < dict_size){
    for (int j = 0; j < word_struct_ptr->prob_list[i].num_of_next_appear; ++j){
      all_next[index]=word_struct_ptr->prob_list[i].word_struct_ptr->word;
      index ++;}
    i++;
  }
  int rand_num = get_random_number(index);
  char *rand_word = all_next[rand_num];
  int num = 0;
  WordStruct *cur_word = NULL;
  while (num <= dict_size) {
    cur_word = word_struct_ptr->prob_list[num].word_struct_ptr;
    if (strcmp(cur_word->word, rand_word) == 0) {
      return cur_word;
    }
    num++;
  }
  return NULL;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * How it works? So we ask for a first random word and print her, then we will
 * ask for next random word, while our len(our_sentence) != 20,and in that time
 * we will check if the next word is with dot or not and choose if to done iter
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary) {
    int num_of_words_in_sen = 1;
    WordStruct *rand_word = get_first_random_word(dictionary);
    printf("%s ", rand_word->word);
    int num = 1;
    while (num < MAX_WORDS_IN_SENTENCE_GENERATION) {
      num++;
      rand_word = get_next_random_word(rand_word);
      if (rand_word != NULL){
        unsigned int index = strlen(rand_word->word) - 1;
        if (rand_word->word[index] == '.' ||
            num == MAX_WORDS_IN_SENTENCE_GENERATION){
          printf("%s", rand_word->word);
          num_of_words_in_sen++;
          break;
        }
        if (rand_word->word[index] != '.') {
          printf("%s ", rand_word->word);
          num_of_words_in_sen++;
        }
      }
    }
//  printf("\n");
  return num_of_words_in_sen;
}



/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct *first_word,WordStruct*second_word)
{
  // If there is no next word for first_word at all, we want to define one:
  if (first_word->word[strlen(first_word->word)-1] == '.'){
    first_word->prob_list = NULL;
    first_word->prob_list_size = 0;
    return 1;}
  if (first_word->prob_list_size ==0){
    first_word->prob_list = malloc(sizeof(WordProbability));
    first_word->prob_list->word_struct_ptr = second_word;

    first_word->prob_list_size++;
    first_word->prob_list->num_of_next_appear = 1;
    return 0;
  }
  // Check if second_word is inside the prob_list of the first_word
  long int prob_size = 0;
  while (prob_size != first_word->prob_list_size){
    if (strcmp(first_word->prob_list[prob_size].word_struct_ptr->word,
               second_word->word) == 0){
      first_word->prob_list[prob_size].num_of_next_appear++;
      return 0;}
    prob_size++;
  }
  /* If the prob_list of the first_word is not empty && second_word is not
   * there we want to enlarge the array with one more WordStruct and add it. */
  prob_size = first_word->prob_list_size+1;
  first_word->prob_list_size++;
  WordProbability *temp = realloc(first_word->prob_list,
                                  (prob_size)*sizeof(WordProbability));
  // check allocation
  if (!temp){
    fprintf(stdout, ALLOCATION_ERROR);
    exit(EXIT_FAILURE);
    }
  first_word->prob_list = temp;
  first_word->prob_list[prob_size-1].word_struct_ptr = second_word;
  first_word->prob_list[prob_size-1].num_of_next_appear = 1;
  return 1;
}



/**
 * Search a word inside the given dictionary
 * @param some dictionary
 * @param word to search for in the given dictionary
 * @return return the node if the word had found in the dictionary, NULL else.
 */
Node *find_node_in_dictionary(LinkList *dictionary, char *word){
  Node *cur_node = dictionary->first;
  int num = 0;
  if (dictionary->size == num){ // the dictionary is empty!
    return NULL;}
  while (num != dictionary->size) {
    if (strcmp(cur_node->data->word, word) == 0){
      return cur_node;}
    cur_node = cur_node->next;
    num++;}
  return NULL;
}


/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary){
  int counter = 0;
  int bool_val = 1;
  char *cur_word;
  WordStruct *word1 = NULL, *word2 = NULL, *new_word = NULL;
  char cur_sentence[MAX_SENTENCE_LENGTH];
  while ((fgets(cur_sentence, MAX_SENTENCE_LENGTH, fp) != NULL)){
    cur_word = strtok(cur_sentence, " \r\n");
    if (counter == words_to_read){return;}
    while ((counter != words_to_read) && (cur_word != NULL)){
      unsigned int cur_word_len = strlen(cur_word) + 1;
      counter++;
      Node *cur_node = find_node_in_dictionary(dictionary, cur_word);
      // the cur_word is in the dictionary, we want to update word_occur
      if (cur_node != NULL){
        cur_node->data->num_of_occur++;
        if (bool_val == 0){
            word2 = cur_node->data;
            add_word_to_probability_list(word1, word2);
            word1 = word2;
        }
      }
      // the cur_word is not in the dictionary so we want to add it!
      if (cur_node == NULL){
        new_word = malloc(sizeof(WordStruct));
        new_word->word = malloc(sizeof(char)*cur_word_len);
        if (new_word->word == NULL){
          fprintf(stdout, ALLOCATION_ERROR);
          exit(1);}
        strcpy(new_word->word, cur_word);
        new_word->prob_list_size = 0;
        new_word->num_of_occur = 1;
        int i = add(dictionary, new_word);
        if (i == 1){ // checks if add func works well
          fprintf(stdout, ADD_FUNC_ERROR);
          exit(EXIT_FAILURE);}
        if (bool_val == 0){
            add_word_to_probability_list(word1, new_word);
            word1 = new_word;
        }
      }
    if (bool_val == 1){
      bool_val = 0;
      word1 = new_word;
    }
    cur_word = strtok(NULL, " \r\n");
    }
  }
}



/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary (LinkList *dictionary) {
  if (dictionary == NULL){
    exit(1);
  }
 Node *cur_node = dictionary->first;
 Node *cur_node_copy = NULL;
 long int dict_size = dictionary->size;
 long int num = 0;
 while (num != dict_size){
     free (cur_node->data->prob_list);
     free (cur_node->data->word);
     free (cur_node->data); // for the add function!
     cur_node_copy = cur_node;
     cur_node = cur_node->next;
     free(cur_node_copy);
     cur_node_copy = NULL;
     num++;
 }
 free(dictionary);
 dictionary = NULL;
}



int check_input_validity(int argc, char *argv[]){
  // check number of arguments
  if (argc != MAX_ARGS_NUM && argc != MIN_ARGS_NUM){
    fprintf(stdout, ARGS_NUM_ERROR);
    return 1;
  }
  // check valid path file
  FILE *text_corpus_file = fopen(argv[3], "r");
  if (text_corpus_file == NULL){
    fprintf(stdout, FILE_P_ERROR);
    return 1;
  }
  fclose(text_corpus_file);
  return 0;
}


/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char *argv[]){
  if (check_input_validity(argc, argv)){
    return EXIT_FAILURE;
  }
  long int seed_val = 0, num_of_tweets = 0, num_of_words_to_read = -1;
  //TODO: to check the validitiy of sscanf
  if (sscanf(argv[1], "%li", &seed_val) == EOF){
    fprintf(stdout, SSCANF_ERROR);
    return EXIT_FAILURE;
  }
  if (sscanf(argv[2], "%li", &num_of_tweets) == EOF){
    fprintf(stdout, SSCANF_ERROR);
    return EXIT_FAILURE;
  }
  if (argc == MAX_ARGS_NUM){
    sscanf(argv[4], "%li", &num_of_words_to_read);
    if (num_of_words_to_read == 0){
      num_of_words_to_read = -1;
    }
  }
  FILE *text_corpus_file = fopen(argv[3], "r");
  LinkList *our_dictionary = malloc(sizeof(LinkList));
  if (our_dictionary == NULL){
    fprintf(stdout, ALLOCATION_ERROR);
    return EXIT_FAILURE;
  }
  // initialize the dictionary data members!
  our_dictionary->size = 0;
  our_dictionary->first = NULL;
  our_dictionary->last = NULL;
  fill_dictionary(text_corpus_file, num_of_words_to_read, our_dictionary);
  srand(seed_val);
  fclose(text_corpus_file);
  for (int i = 0; i < num_of_tweets; i++){
    printf("Tweet %d: ", i+1);
    generate_sentence(our_dictionary);
    if (i != num_of_tweets-1){
      printf("\n");
    }
  }
  free_dictionary(our_dictionary);
  return EXIT_SUCCESS;
}