touch KoshTest
echo "Вот этот текст не видно будет" > KoshTest
echo "А этот текст будет видно" > KoshTest
echo "И этот тоже" >> KoshTest
cat < KoshTest > Kosh2Test
echo "KoshTest file:"
cat KoshTest
echo "Kosh2Test file:"
cat Kosh2Test
rm KoshTest; rm Kosh2Test;
echo "Несколько sleep:"
sleep 3 & sleep 3 & sleep 3 &
echo "12" | cat &
