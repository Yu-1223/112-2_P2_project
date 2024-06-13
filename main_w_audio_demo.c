#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "toml.h"
#include "display_interface.h"

#undef DEBUG

char *dialogueKeys[] = {"home", "clinic", "park", "ending"};
char *sectionKeys[][7] = 
{
    {"start", "found_banana_cat", "pick_up_smoothing_ball", "ignore_smoothing_ball", "\n", "\n", "\n"},
    {"arrived_clinic", "received_treatment", "not_received_treatment", "\n", "\n", "\n", "\n"},
    {"arrived_park", "join_game", "not_join_game", "play_game", "playing_hide_and_seek", "playing_alone", "playing_tag"},
    {"1", "2", "3", "\n", "\n", "\n", "\n"}
};

char *backpackString[] = {"Smothing ball", "healthy snack", "Smothing ball, healthy snack", " "};

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
    for(int32_t i = 0 ; i<7 ; i++)
    {
        if(strcmp(sectionKeys[dialogueIndex][i], sectionKey) == 0)
        {
            return i;
        }
    }
}

void play_sound(Mix_Chunk *sound) {
    int channel = Mix_PlayChannel(-1, sound, 0);
    if (channel == -1) {
        fprintf(stderr, "Failed to play sound! SDL_mixer Error: %s\n", Mix_GetError());
    }

    while (Mix_Playing(channel) != 0) {
        SDL_Delay(100); // 每 100 毫秒檢查一次音效是否播放結束
    }
}

int main() 
{
    // 初始化 SDL 音頻子系統
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        error("Could not initialize SDL: ", SDL_GetError());
    }

    // 初始化 SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        error("SDL_mixer could not initialize! SDL_mixer Error: ", Mix_GetError());
    }

    // 加載音效檔案
    Mix_Chunk *homeMusic = Mix_LoadWAV("/path/to/home_music.wav");
    Mix_Chunk *clinicMusic = Mix_LoadWAV("/path/to/clinic_music.wav");
    Mix_Chunk *parkMusic = Mix_LoadWAV("/path/to/park_music.wav");
    Mix_Chunk *backpackOpenSound = Mix_LoadWAV("/path/to/backpack_open.wav");
    Mix_Chunk *backpackCloseSound = Mix_LoadWAV("/path/to/backpack_close.wav");

    if (!homeMusic || !clinicMusic || !parkMusic || !backpackOpenSound || !backpackCloseSound) {
        error("Failed to load sound! SDL_mixer Error: ", Mix_GetError());
    }

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
    char *image = (char *)malloc(50 * sizeof(char));
    char *backpackImage = (char *)malloc(50 * sizeof(char));
    char *desString = (char *)malloc(50 * sizeof(char));  
    char *desString2 = (char *)malloc(50 * sizeof(char));
    char *optionNext = (char *)malloc(50 * sizeof(char));
    char *option1Text = (char *)malloc(10 * sizeof(char));
    char *option2Text = (char *)malloc(10 * sizeof(char));
    int32_t jumpFlag = 0; // the flag for jumping after choosing the option 
    int32_t promptFlag = 0;
    int32_t userOptions = 0;
    int32_t prevK = 0;
    int32_t haveItem1 = 0;
    int32_t haveItem2 = 0;
    int32_t usedItem1 = 0;
    int32_t usedItem2 = 0;
    int32_t affectionPoint = 0;
    char *backpackItemList = backpackString[3];

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

        // 播放場景音樂
        if (strcmp(dialogueKey, "home") == 0) {
            play_sound(homeMusic);
        } else if (strcmp(dialogueKey, "clinic") == 0) {
            play_sound(clinicMusic);
        } else if (strcmp(dialogueKey, "park") == 0) {
            play_sound(parkMusic);
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

                if(strcmp(key, "pick_up_smoothing_ball") == 0 && usedItem1 == 0)
                {
                    haveItem1 = 1;
                    backpackItemList = backpackString[0];
                }
                if(strcmp(key, "received_treatment") == 0 && usedItem2 == 0)
                {
                    haveItem2 = 1;

                    if(haveItem1 == 0)
                    {
                        backpackItemList = backpackString[1];
                    }
                    else
                    {
                        backpackItemList = backpackString[2];
                    }
                }

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
                            play_sound(backpackOpenSound); // 播放背包打開音效

                            if(haveItem1 == 0 && haveItem2 == 0)
                            {
                                backpackImage = "";
                                backpackItemList = backpackString[3];
                            }
                            else if(haveItem1 == 1 && haveItem2 == 0)
                            {
                                backpackImage = "";
                                backpackItemList = backpackString[0];
                            }
                            else if(haveItem1 == 0 && haveItem2 == 1)
                            {
                                backpackImage = "";
                                backpackItemList = backpackString[1];
                            }
                            else if(haveItem1 == 1 && haveItem2 == 1)
                            {
                                backpackImage = "";
                                backpackItemList = backpackString[2];
                            }

                            display_interface(backpackImage, "backpack Opened! Press \"b\" again to close or choose to apply item!", userOptions, backpackItemList, "Sad", "Happy", "Boring");

                            do
                            {
                                if(haveItem1 == 0 && haveItem2 == 0)
                                {
                                    userOptions = get_option("", "");
                                }
                                else if(haveItem1 == 1 && haveItem2 == 0)
                                {
                                    userOptions = get_option("Smoothing ball", "");
                                }
                                else if(haveItem1 == 0 && haveItem2 == 1)
                                {
                                    userOptions = get_option("", "Healthy snack");
                                }
                                else if(haveItem1 == 1 && haveItem2 == 1)
                                {
                                    userOptions = get_option("Smoothing ball", "Healthy snack");
                                }

                            }while(userOptions != 1 && userOptions != 2 && userOptions != 4);

                            if(userOptions == 1)
                            {
                                if(haveItem1 == 1)
                                {
                                    display_interface(backpackImage, "item applied success!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                    haveItem1 = 0;
                                    usedItem1 = 1;
                                    affectionPoint += 10;

                                    if(haveItem2 == 0)
                                    {
                                        backpackItemList = backpackString[3];
                                    }
                                    else
                                    {
                                        backpackItemList = backpackString[1];
                                    }
                                }
                                else
                                {
                                    display_interface(backpackImage, "item not found!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                }
                                sleep(1);
                            }
                            else if(userOptions == 2)
                            {
                                if(haveItem2 == 1)
                                {
                                    display_interface(backpackImage, "item applied success!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                    haveItem2 = 0;
                                    usedItem2 = 1;
                                    affectionPoint += 10;

                                    if(haveItem1 == 0)
                                    {
                                        backpackItemList = backpackString[3];
                                    }
                                    else
                                    {
                                        backpackItemList = backpackString[0];
                                    }
                                }
                                else
                                {
                                    display_interface(backpackImage, "item not found!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                }
                                sleep(1);
                            }
                            else if(userOptions == 4)
                            {
                                display_interface(backpackImage, "backpack Closed!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                play_sound(backpackCloseSound); // 播放背包關閉音效
                                sleep(1);
                            }

                            userOptions = 3;
                            k = prevK; // go back to the previous dialogue
                        }
                        
                        if(userOptions == 3 || promptFlag == 1)
                        {
                            const toml_table_t* subtable = toml_table_at(array, k);
                            promptFlag = 0;

                            beforeBackpack: // create label

                            if(subtable) 
                            {
                                // parse display text
                                const char* character = toml_raw_in(subtable, "character");
                                const char* text = toml_raw_in(subtable, "text");
                                const char* background = toml_raw_in(subtable, "background");

                                if(background)
                                {
                                    image = background;
                                    image[strlen(image) - 1] = '\0'; // get rid of ""
                                }

                                if(character && text) 
                                {
                                    #ifdef DEBUG
                                        printf("Character: %s, Text: %s\n", character, text);
                                    #endif 

                                    display_interface((char *)image + 1, (char *)text, userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                }

                                const char* next = toml_raw_in(subtable, "next");
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

                                    // Check if next is "finish"
                                    if(strcmp(desString, "finish") == 0) 
                                    {
                                        sleep(2);
                                        // Clear windows
                                        system("clear");
                                        // Exit program
                                        Mix_FreeChunk(homeMusic);
                                        Mix_FreeChunk(clinicMusic);
                                        Mix_FreeChunk(parkMusic);
                                        Mix_FreeChunk(backpackOpenSound);
                                        Mix_FreeChunk(backpackCloseSound);
                                        Mix_CloseAudio();
                                        SDL_Quit();
                                        exit(EXIT_SUCCESS);
                                    }

                                    // Check if next is "ending" and set the appropriate ending based on affectionPoint
                                    if(strcmp(desString, "ending") == 0) 
                                    {
                                        if(affectionPoint == 0) 
                                        {
                                            strcpy(desString2, "1");
                                        } 
                                        else if(affectionPoint == 10) 
                                        {
                                            strcpy(desString2, "2");
                                        } 
                                        else if(affectionPoint == 20) 
                                        {
                                            strcpy(desString2, "3");
                                        }
                                    }

                                    jumpFlag = 1;
                                    break;
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
                                    background = toml_raw_in(promptText, "background");
                                    
                                    if(background)
                                    {
                                        image = background;
                                        image[strlen(image) - 1] = '\0'; // get rid of ""
                                    }

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
                                            play_sound(backpackOpenSound); // 播放背包打開音效

                                            if(haveItem1 == 0 && haveItem2 == 0)
                                            {
                                                backpackImage = "";
                                                backpackItemList = backpackString[3];
                                            }
                                            else if(haveItem1 == 1 && haveItem2 == 0)
                                            {
                                                backpackImage = "";
                                                backpackItemList = backpackString[0];
                                            }
                                            else if(haveItem1 == 0 && haveItem2 == 1)
                                            {
                                                backpackImage = "";
                                                backpackItemList = backpackString[1];
                                            }
                                            else if(haveItem1 == 1 && haveItem2 == 1)
                                            {
                                                backpackImage = "";
                                                backpackItemList = backpackString[2];
                                            }

                                            display_interface(backpackImage, "backpack Opened! Press \"b\" again to close or choose to apply item!", userOptions, backpackItemList, "Sad", "Happy", "Boring");

                                            do
                                            {
                                                if(haveItem1 == 0 && haveItem2 == 0)
                                                {
                                                    userOptions = get_option("", "");
                                                }
                                                else if(haveItem1 == 1 && haveItem2 == 0)
                                                {
                                                    userOptions = get_option("Smoothing ball", "");
                                                }
                                                else if(haveItem1 == 0 && haveItem2 == 1)
                                                {
                                                    userOptions = get_option("", "Healthy snack");
                                                }
                                                else if(haveItem1 == 1 && haveItem2 == 1)
                                                {
                                                    userOptions = get_option("Smoothing ball", "Healthy snack");
                                                }

                                            }while(userOptions != 1 && userOptions != 2 && userOptions != 4);

                                            if(userOptions == 1)
                                            {
                                                if(haveItem1 == 1)
                                                {
                                                    display_interface(backpackImage, "item applied success!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                                    haveItem1 = 0;
                                                    usedItem1 = 1;
                                                    affectionPoint += 10;

                                                    if(haveItem2 == 0)
                                                    {
                                                        backpackItemList = backpackString[3];
                                                    }
                                                    else
                                                    {
                                                        backpackItemList = backpackString[1];
                                                    }
                                                }
                                                else
                                                {
                                                    display_interface(backpackImage, "item not found!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                                }
                                                sleep(1);
                                            }
                                            else if(userOptions == 2)
                                            {
                                                if(haveItem2 == 1)
                                                {
                                                    display_interface(backpackImage, "item applied success!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                                    haveItem2 = 0;
                                                    usedItem2 = 1;
                                                    affectionPoint += 10;

                                                    if(haveItem1 == 0)
                                                    {
                                                        backpackItemList = backpackString[3];
                                                    }
                                                    else
                                                    {
                                                        backpackItemList = backpackString[0];
                                                    }
                                                }
                                                else
                                                {
                                                    display_interface(backpackImage, "item not found!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                                }
                                                sleep(1);
                                            }
                                            else if(userOptions == 4)
                                            {
                                                display_interface(backpackImage, "backpack Closed!", userOptions, backpackItemList, "Sad", "Happy", "Boring");
                                                play_sound(backpackCloseSound); // 播放背包關閉音效
                                                sleep(1);
                                            }

                                            goto beforeBackpack; // jump to label (code before open backpack) -> use to go back to the previous dailogue
                                        }
                        
                                        // display prompt text
                                        if(userOptions == 3)
                                        {
                                            display_interface((char *)image + 1, (char *)promptQ, userOptions, backpackItemList, "Sad", "Happy", "Boring");
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
                                            printf("des1: %s, %s\n", desString, desString2);
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
    free(image);
    free(backpackImage);
    toml_free(conf);

    // 釋放資源並關閉 SDL_mixer 和 SDL
    Mix_FreeChunk(homeMusic);
    Mix_FreeChunk(clinicMusic);
    Mix_FreeChunk(parkMusic);
    Mix_FreeChunk(backpackOpenSound);
    Mix_FreeChunk(backpackCloseSound);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
