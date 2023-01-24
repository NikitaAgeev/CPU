#ifndef ONEGIN_HED_H
#define ONEGIN_HED_H
/*!
Группа стандарных сообщений об ошибках для многих функций
*/
enum STD_ERRORS
{
    CANT_REALOK_MEMORY = -1, //< Невозможно выделить память
    INVALID_FILE_LEN = -2,   //< Ошибка в определении длины файла
    INVALID_FIILE_STAT = -3, //< Невозможно определить характеристики файла
    OK = 0                   //< Всё OK
};

/*!
Вывод функции, определяющий является ли буквой символ
*/
enum SIMVOL_CHEK_RESULT
{
    IT_IS_SIMVOL = 1,   //< Это буква
    IT_IS_NO_SIMVOL = 0 //< Это не буква
};

/*!
Вывод функции сравнения
*/
enum CMP_RESULT
{
    A_LAGER_B =  1, //< A > B
    B_LAGER_A = -1, //< A < B
    A_EQ_B    =  0  //< A = B
};




/*!
Структура, хранящая в сбе указатель на стоку и иё длину
*/
typedef struct
{
    size_t len;        //< Длина строки
    char* str;         //< Указатель на первый элемент строки
} string;

/*!
Структура хранящая указатель на буфер текста и его параметры
*/
typedef struct
{
    size_t len;        //< Длина буфера текста в символах
    char* txt;         //< Указатель на буфер текста  
    size_t str_count;  //< Количество строк в тексте
    string* str_mass;  //< Указатель на масив строк текста
} Text ;




/*!
Автоматически определяет длину файла, выделяет память для сохранения файла и записывает указатель на неё в copy.
\param[out]  copy  указатель на строчку для записи содержимого файла
\param[out]  len   указатель на переменную для записи длинны полученной строки
\param[in]   file  указатель на файл, который нужно считать
\return      int   код ошибки соответвуя STD_ERRORS
*/
int  full_file_reader  (char** copy, size_t* len, FILE* file);

/*!
Находит количество строк, выделяет память для массива строк, делит на строки,
В каждой строке преобразует '\n' в '\0', записывает указатель первого непробельного символа, и устанавливает сивол
конца строки сразу же после последнего непробельного символа в ней (для этого также изменяет буфер текста)  
\param[in] text  текст для преобразования
\return    int   сообщения об ошибках в соответствии с STD_ERRORS
*/
int  text_normificator (Text* text);

/*!
Считывает текст из файла и преобразует к нормальному виду
\param[out]  text  указатель на структуру текста
\param[in]   file  указатель на файл для считывания
\return      int   сообщения об ошибках в соответствии с STD_ERRORS
*/
int  text_reader       (Text* text, FILE* file);

/*!
Считает количество строк в буфере текста и записывает результат в соответсвующее поле структуры
\param[in] text структура текста для анализа
*/
void str_count         (Text* text);


/*!
Сравнивает строки игнорируя все символы кроме букв и цифр
\param  str_A строка A для сравнения
\param  str_B строка B для сравнения
\return В соответствии с CMP_RESULT
*/
int  str_cmp_plus      (void* str_Av, void* str_Bv);

/*!
Сравнивает строки с конца игнорируя все символы кроме букв и цифр
\param  str_A строка A для сравнения
\param  str_B строка B для сравнения
\return В соответствии с CMP_RESULT
*/
int  str_rcmp_plus     (const void* str_Av, const void* str_Bv);

/*!
Проверяет символ на то, является ли он буквой или цифрой
\param ch символ для проверки 
\return результат в соответствии с CMP_RESULT
*/
int  it_is_simvol      (char ch);

/*!
Производит быструю сортировку подаваемого масива(фактически является обёрткой для qsort_c)
\param[in] first начало сортируемого масива
\param[in] number количество элементов которе нужно отсортировать
\param[in] размер элемента
\param[in] comparator указатель на функцию сравнения
\return код ошибки в соответствии с STD_ERRORS
*/
int  qsort_bd          (void * first, size_t number, size_t size, int ( * comparator ) ( const void *, const void * ));

/*!
Производит быструю сортировку подаваемого масива(фактически является обёрткой для qsort_c)
\param[in] first начало сортируемого масива
\param[in] number количество элементов которе нужно отсортировать
\param[in] размер элемента
\param[in] comparator указатель на функцию сравнения
\param[in] pivot память для pivot элемента
\return код ошибки в соответствии с STD_ERRORS
*/
void qsort_c           ( void * first, size_t number, size_t size, int ( * comparator ) ( const void *, const void * ), void* pivot);

/*!
Эфективно(вроде) меняет элементы между собой
\param[in] a, b элементы для смены
\param[in] size размер элемента
*/
void swap_c            (void* a, void* b, size_t size);


/*!
Записывает строку целиком, игнорируя(не записывая \0) и убирая лишние пробелы
\param str строка для вывода 
\param stream поток вывода данных
\return сообщения о ошибках
*/
void str_print_plus    (string str, FILE* stream);

/*!
Выводет текст по массиву строк, игнорируя(не записывая \0) и лишние пробелы
\param text текст для вывода
\param stream поток для вывода
*/
void text_print_plus   (Text text, FILE* stream);

/*!
Записывает в stream исходный текст игнорируя лишние ведущие пробелы и конечные
\param text текст для вывода
\param stream поток для вывода
*/
void txt_buf_printer   (Text text, FILE* stream);


/*!
Печать для дебага, позволяет увидеть строку полностью
\param[in] str строка для печати
\param[in] start начало строки
\param[in] end конец строки
\param[in] line место для указания строки дебага)
\param[in] funk имя функции где дебаг
*/
void debag_char_print  (char* str, size_t start, size_t end, size_t line, char* func);

/*!
*/
int  debug_text_print  (Text* text);

#endif