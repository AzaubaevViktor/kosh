touch KoshTest
echo "Вот этот текст не видно будет" > KoshTest
echo "А этот текст будет видно" > KoshTest
echo "И этот тоже" >> KoshTest
cat < KoshTest > Kosh2Test

echo "KoshTest file:"
echo "============================="
cat KoshTest
echo "-----------------------------"

echo "Kosh2Test file:"
echo "============================="
cat Kosh2Test
echo "-----------------------------"

rm KoshTest; rm Kosh2Test;

echo "НЕСКОЛЬКО SLEEP И КОМАНДА НЕ В ФОНЕ:"
sleep 3 & echo "I'm not background, sleep 1"; sleep 1; sleep 3 & sleep 3 &

echo "МНОЖЕТСВЕННОЕ ПЕРЕНАПРАВЛЕНИЕ"
echo "12" | cat | cat | cat

echo "УПРАВЛЕНИЕ ЗАДАНИЯМИ"
echo "Создаём несколько задач в фоне"
sleep 1000 & sleep 2000 & sleep 3000 & sleep 4000 &
jobs
echo "Сделаем активной последнее запущенное задание и нажмём ^Z"
fg
echo "Посмотрим теперь на задачи, одна из них должна быть stopped"
jobs
echo "А теперь запустим предпоследнее задание и снова нажмём ^Z"
fg -
echo "+ и - переместились:"
jobs
echo "А теперь 4-е задание мы активируем (и команда скажет, что оно уже запущено)"
bg 4
echo "А теперь по имени:"
bg "sleep"
echo "Уточним:"
bg "sleep 1"
echo "Выходим"
exit
