typedef struct student student;
struct student {
    float gpa;
    char* grade;
    char* name ;
};

student* generate_random_students_list(size_t num_students);

int compare_students_by_name(void const* a, void const *b);

int compare_students_by_gpa(void const* a, void const *b);