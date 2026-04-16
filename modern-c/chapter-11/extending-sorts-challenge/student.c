#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "student.h"
#include "rng64.h"

const char* first_names[100] = {
"Alice","Bob","Charlie","Diana","Ethan","Fiona","George","Hannah","Ian","Julia",
"Kevin","Laura","Michael","Nina","Oscar","Paula","Quinn","Rachel","Sam","Tina",
"Umar","Vera","Will","Xena","Yara","Zane","Aaron","Bianca","Caleb","Delia",
"Eli","Farah","Gavin","Hazel","Isaac","Jade","Kyle","Lena","Mason","Nora",
"Owen","Piper","Quincy","Rita","Sean","Tara","Ulric","Valerie","Wade","Xavier",
"Yvonne","Zach","Adrian","Bella","Carter","Derek","Elena","Felix","Grace","Henry",
"Ivy","Jonah","Kara","Leo","Mila","Noah","Olive","Peter","Queen","Ralph",
"Sara","Tom","Ursula","Victor","Willa","Ximena","Yusuf","Zoey","Aiden","Brooke",
"Colin","Daisy","Evan","Faith","Gabe","Hope","Iris","Jack","Kurt","Lily",
"Max","Nate","Opal","Paul","Quinn","Rose","Scott","Tracy","Uri","Vince"
};

const char* last_names[100] = {
"Smith","Johnson","Williams","Brown","Jones","Garcia","Miller","Davis","Rodriguez","Martinez",
"Hernandez","Lopez","Gonzalez","Wilson","Anderson","Thomas","Taylor","Moore","Jackson","Martin",
"Lee","Perez","Thompson","White","Harris","Sanchez","Clark","Ramirez","Lewis","Robinson",
"Walker","Young","Allen","King","Wright","Scott","Torres","Nguyen","Hill","Flores",
"Green","Adams","Nelson","Baker","Hall","Rivera","Campbell","Mitchell","Carter","Roberts",
"Gomez","Phillips","Evans","Turner","Diaz","Parker","Cruz","Edwards","Collins","Reyes",
"Stewart","Morris","Morales","Murphy","Cook","Rogers","Gutierrez","Ortiz","Morgan","Cooper",
"Peterson","Bailey","Reed","Kelly","Howard","Ramos","Kim","Cox","Ward","Richardson",
"Watson","Brooks","Chavez","Wood","James","Bennett","Gray","Mendoza","Ruiz","Hughes",
"Price","Alvarez","Castillo","Sanders","Patel","Myers","Long","Ross","Foster","Jimenez"
};

const char* YEAR_NAMES[] = {
    "Freshman",
    "Sophomore",
    "Junior",
    "Senior"
};

student create_student(){
    student new_student = {};
    rng64_intrange_spec  r;
    rng64_set_intrange(&r, 1, 5);

    new_student.gpa = rng64_one();

    rng64_set_intrange(&r, 0, 99);
    int first_index = (int)rng64_intrange(&r);
    int second_index = (int)rng64_intrange(&r);

    const char* first_name = first_names[first_index];
    const char* last_name  = last_names[second_index];

    size_t len_first = strlen(first_name);
    size_t len_last  = strlen(last_name);

    char* new_name = malloc(len_first + 1 + len_last + 1);

    memcpy(new_name, first_name, len_first);
    new_name[len_first] = ' ';
    memcpy(new_name + len_first + 1, last_name, len_last);
    new_name[len_first + 1 + len_last] = '\0';

    new_student.name = new_name;

    rng64_set_intrange(&r, 0, 3);
    int grade = (int)rng64_intrange(&r);

    new_student.grade = YEAR_NAMES[grade];

    return new_student;
}

student* generate_random_students_list(size_t num_students){
    rng64_randomize();

    student* generated_students = malloc(sizeof(student) * num_students);

    size_t current_index = 0;

    while (current_index < num_students) {
        student new_student = create_student();
        generated_students[current_index] = new_student;
        current_index++;
    }

    return generated_students;
}

int compare_students_by_name(void const* a, void const *b){
    student const* student_a = a;
    student const* student_b = b;

    int result = strcmp(student_a->name, student_b->name);

    return result;
}

int compare_students_by_gpa(void const* a, void const *b){
    student const* student_a = a;
    student const* student_b = b;

    if (student_a->gpa < student_b->gpa) {
        return -1;
    }
    if (student_a->gpa > student_b->gpa) {
        return 1;
    }
    return 0;
}