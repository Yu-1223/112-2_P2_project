#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "toml.h"
#include "display_interface.h"

#undef DEBUG

char *dialogueKeys[] = {"home", "clinic", "park", "ending"};
char *sectionKeys[][3] = 
{
    {"start", "found_crying_cat", "used_soothing_ball"},
    {"arrived_clinic", "received_treatment", "\0"},
    {"arrived_park", "\0", "\0"},
    {"1", "2", "3"}
};

// Error handling function
void error(const char* msg, const char* detail) 
{
    fprintf(stderr, "%s%s\n", msg, detail);
    exit(EXIT_FAILURE);
}

int32_t getDialogueIndex(const char *dialogueKey)
{
    for(int32_t i = 0 ; i<4 ; i++)
    {
        if(strcmp(dialogueKeys[i], dialogueKey) == 0)
        {
            return i;
        }
    }
}

int32_t getSectionIndex(int32_t dialogueIndex, const char *sectionKey)
{
    for(int32_t i = 0 ; i<3 ; i++)
    {
        if(strcmp(sectionKeys[dialogueIndex][i], sectionKey) == 0)
        {
            return i;
        }
    }
}


int main() 
{
    FILE* fp;
    char errbuf[200];

    // 1. Read and parse toml file
    fp = fopen("script.toml", "r");
    if (!fp) {
        error("cannot open script.toml - ", strerror(errno));
    }

    toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if(!conf) 
    {
        error("cannot parse - ", errbuf);
    }

    // 2. Traverse table dialogue
    const toml_table_t* dialogue = toml_table_in(conf, "dialogue");

    if(!dialogue) 
    {
        error("missing [dialogue]", "");
    }

    const char* dialogueKey;
    char *desString = (char *)malloc(50 * sizeof(char));  
    char *desString2 = (char *)malloc(50 * sizeof(char));
    char *optionNext = (char *)malloc(50 * sizeof(char));
    char *option1Text = (char *)malloc(10 * sizeof(char));
    char *option2Text = (char *)malloc(10 * sizeof(char));
    int32_t jumpFlag = 0; // the flag for jumping after choosing the option 
    int32_t promptFlag = 0;
    int32_t userOptions = 0;
    int32_t prevK = 0;

    display_interface("", "", 0, "", "", "", "");

    // iterate through every scene ("home", "clinic", "park", "ending")
    for(int32_t i = 0 ; (dialogueKey = toml_key_in(dialogue, i)); i++) 
    {
        if(jumpFlag == 1)
        {
            dialogueKey = desString;
            #ifdef DEBUG
                printf("jump : %s\n", dialogueKey);
            #endif 
            i = getDialogueIndex(dialogueKey); // move the index to the jump destination
        }

        const toml_table_t* section = toml_table_in(dialogue, dialogueKey);

        // iterate through every section in scene ("home", "clinic", "park", "ending")
        if(section) 
        {
            #ifdef DEBUG
                printf("Section: %s\n", dialogueKey);
            #endif  
            const char* key;
            toml_array_t* array;

            // iterate through the text of the corresponding section        
            for(int32_t j = 0 ; (key = toml_key_in(section, j)) ; j++) 
            {
                if(jumpFlag == 1)
                {
                    jumpFlag = 0;
                    key = desString2;
                    #ifdef DEBUG
                        printf("jump: %s\n", key);
                    #endif  
                    j = getSectionIndex(i, key); // move the index to the jump destination
                    
                }

                #ifdef DEBUG
                    printf("Key: %s\n", key);
                #endif 
                array = toml_array_in(section, key);

                if(array) 
                {
                    for(int k = 0; k < toml_array_nelem(array); k++) 
                    {
                        if(promptFlag != 1) // do not need to press 'a' after select option
                        {
                            do
                            {
                                userOptions = get_option(" ", " ");

                            }while(userOptions != 3 && userOptions != 4);
                        }

                        if(userOptions == 4) // open backpack
                        {
                            display_interface("cat.jpg", "backpack Opened! Press \"b\" again to close or chooes to apply item!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");

                            do
                            {
                                userOptions = get_option("item1", "item2");

                            }while(userOptions != 1 && userOptions != 2 && userOptions != 4);

                            if(userOptions == 1)
                            {
                                display_interface("cat.jpg", "item 1 applied success!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                sleep(1);
                            }
                            else if(userOptions == 2)
                            {
                                display_interface("cat.jpg", "item 2 applied success!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                sleep(1);
                            }
                            else if(userOptions == 4)
                            {
                                display_interface("cat.jpg", "backpack Closed1!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                sleep(1);
                            }

                            userOptions = 3;
                            k = prevK;
                        }
                        
                        if(userOptions == 3 || promptFlag == 1)
                        {
                            const toml_table_t* subtable = toml_table_at(array, k);
                            promptFlag = 0;

                            beforeBackpack: // create label

                            if(subtable) 
                            {
                                // parse display text
                                const char* charactor = toml_raw_in(subtable, "charactor");
                                const char* text = toml_raw_in(subtable, "text");

                                if(charactor && text) 
                                {
                                    #ifdef DEBUG
                                        printf("Character: %s, Text: %s\n", charactor, text);
                                    #endif 

                                    display_interface("cat.jpg", (char *)text, userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                }

                                // check if prompt exist
                                const toml_array_t* prompt = toml_array_in(subtable, "prompt");

                                if(prompt) 
                                {
                                    #ifdef DEBUG
                                        printf("prompt exist!\n");
                                    #endif 

                                    const toml_table_t* promptText = toml_table_at(prompt, 0);
                                    const char *promptQ = toml_raw_in(promptText, "text");

                                    if(promptQ)
                                    {
                                        #ifdef DEBUG
                                            printf("Prompt Text: %s\n", promptQ);
                                        #endif 

                                        do
                                        {
                                            userOptions = get_option(" ", " ");

                                        }while(userOptions != 3 && userOptions != 4);

                                        if(userOptions == 4) // open backpack
                                        {
                                            display_interface("cat.jpg", "backpack Opened! Press \"b\" again to close or chooes to apply item!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");

                                            do
                                            {
                                                userOptions = get_option("item1", "item2");

                                            }while(userOptions != 1 && userOptions != 2  && userOptions != 4);

                                            if(userOptions == 1)
                                            {
                                                display_interface("cat.jpg", "item 1 applied success!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                                sleep(1);
                                            }
                                            else if(userOptions == 2)
                                            {
                                                display_interface("cat.jpg", "item 2 applied success!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                                sleep(1);
                                            }
                                            else if(userOptions == 4)
                                            {
                                                display_interface("cat.jpg", "backpack Closed!", userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                                sleep(1);
                                            }

                                            goto beforeBackpack; // jump to label (code before open backpack)
                                        }
                        
                                        // display prompt text
                                        if(userOptions == 3)
                                        {
                                            display_interface("cat.jpg", (char *)promptQ, userOptions, "Soothing ball", "Sad", "Happy", "Boring");
                                        }
                                    }

                                    const toml_table_t* promptTable1 = toml_table_at(prompt, 1);
                                    const toml_table_t* promptTable2 = toml_table_at(prompt, 2);
                                    memset(option1Text, '\0', 10);
                                    memset(option2Text, '\0', 10);

                                    option1Text = toml_raw_in(promptTable1, "text");
                                    option2Text = toml_raw_in(promptTable2, "text");

                                    option1Text[strlen(option1Text) - 1] = '\0'; // get rid of ""
                                    option2Text[strlen(option2Text) - 1] = '\0'; // get rid of ""

                                    do
                                    {
                                        userOptions = get_option(option1Text + 1, option2Text + 1);

                                    }while(userOptions != 1 && userOptions != 2);

                                    promptFlag = 1;
                                    const char* next;

                                    if(userOptions == 1)
                                    {
                                        next = toml_raw_in(promptTable1, "next");
                                    }
                                    else if(userOptions == 2)
                                    {
                                        next = toml_raw_in(promptTable2, "next");
                                    }
                                    
                                    if(next) 
                                    {
                                        // get the jump destination
                                        memset(desString, '\0', 50);
                                        memset(desString2, '\0', 50);
                                        memset(optionNext, '\0', 50);

                                        #ifdef DEBUG
                                            printf("Next: %s\n", next);
                                        #endif 

                                        strncpy(optionNext, next, strlen(next));
                                        optionNext[strlen(optionNext) - 1] = '\0';
                                        sscanf(optionNext + 1, "%49[^.].%49s", desString, desString2);

                                        #ifdef DEBUG
                                            printf("des: %s, %s\n", desString, desString2);
                                        #endif 

                                        jumpFlag = 1;
                                        break;
                                    }
                                    
                                }
                            }
                        }

                        prevK = k;
                    }

                    if(jumpFlag == 1)
                    {
                        // store the destination
                        dialogueKey = desString;
                        key = desString2;
                        break;
                    }
                }
            }
        }
    }

    free(desString);
    free(desString2);
    free(optionNext);
    free(option1Text);
    free(option2Text);
    toml_free(conf);

    return 0;
}