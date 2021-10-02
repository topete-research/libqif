"""
Bayes risk.
"""
from .. import typing as t

def add_leakage(pi: t.ndarray, C: t.ndarray) -> t.FloatOrRat: ...

def mult_capacity(C: t.ndarray, method: str = 'direct') -> t.Tuple[t.FloatOrRat, t.ndarray]: ...

def mult_leakage(pi: t.ndarray, C: t.ndarray) -> t.FloatOrRat: ...

def posterior(pi: t.ndarray, C: t.ndarray) -> t.FloatOrRat: ...

def prior(pi: t.ndarray) -> t.FloatOrRat: ...

def strategy(pi: t.ndarray, C: t.ndarray) -> t.ndarray: ...
