from translate import translate_content
import os
import glob

for f in glob.glob("*.morse"):
    if f == "hola_mundo.morse":
        continue # already done
    with open(f, 'r') as fp:
        old_data = fp.read()
    new_data = translate_content(old_data)
    with open(f, 'w') as fp:
        fp.write(new_data)
    print(f"Translated {f}")
