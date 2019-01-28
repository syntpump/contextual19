#include <stdio.h>
#include <stdlib.h>
#include "ctxparser.h"
#include "files.h"

struct RulesAdjacentList* newRulesList(){
	struct RulesAdjacentList* list = (struct RulesAdjacentList*)malloc(
		sizeof(struct RulesAdjacentList)
	);
	list->head = NULL;
	return list;
}

struct RulesListNode* newRulesNode(int index){
	struct RulesListNode* node = (struct RulesListNode*)malloc(
		sizeof(struct RulesListNode)
	);
	node->next = NULL;
	node->occurenceIndex = index;
	return node;
}

void appendRule(struct RulesAdjacentList* list, int index){
	struct RulesListNode* node = newRulesNode(index);

	if(list->head == NULL){
		list->head = list->tail = node;
	}else {
		list->tail->next = node;
		list->tail = node;
	}
}

void printList(struct RulesAdjacentList* list){
	struct RulesListNode* ptr = list->head;
	while(ptr != NULL){
		printf("%d\n", ptr->occurenceIndex);
		ptr = ptr->next;
	}
}

struct RulesAdjacentList* findRules(FILE* inpfile){
	struct RulesAdjacentList* list = newRulesList();
	fseek(inpfile, 0, SEEK_SET);
	int position;
	do appendRule(
        list,
        position = seekUntil("if\n", inpfile)
    );
	while (position != NULL);
	return list;
}
