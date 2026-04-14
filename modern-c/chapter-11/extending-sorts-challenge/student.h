typedef struct student student;
struct student {
    float gpa;
    char* grade;
    char* name ;
};

student* generate_random_students_list(size_t num_students);