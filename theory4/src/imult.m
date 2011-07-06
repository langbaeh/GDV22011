function im = imult(a,b)
    im = [ min([a(1)*b(1), a(1)*b(2), a(2)*b(1), a(2)*b(2)]) max([a(1)*b(1), a(1)*b(2), a(2)*b(1), a(2)*b(2)])];