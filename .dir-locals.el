((c-mode . ((flycheck-clang-include-path . ("raylib/src"))
            (flycheck-gcc-include-path . ("raylib/src"))
            (eval . (setq-local company-clang-arguments
                                (list (concat "-I" (expand-file-name "raylib/src" (projectile-project-root)))))))))

