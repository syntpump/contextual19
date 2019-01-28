#define CTXBUFFERSIZE 50

struct RulesAdjacentList{
	struct RulesListNode* head;
	struct RulesListNode* tail;
};

struct RulesListNode{
	int occurenceIndex;
	struct RulesListNode* next;
};

struct RulesAdjacentList* findRules(FILE* inpfile);

void printList(struct RulesAdjacentList* list);
